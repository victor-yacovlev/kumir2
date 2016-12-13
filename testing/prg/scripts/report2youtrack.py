#!/usr/bin/python2
#coding=utf-8


import argparse
import logging
import string
import sys
import os
import re
import urllib
import urllib2
import xml.etree.ElementTree

parser = argparse.ArgumentParser(description=u"Выполняет разбор отчета о тестировании и отправляет задачи в багрепорт")
parser.add_argument("-d", "--debug",
                    help=u"Отображать сообщения отладки скрипта",
                    dest='debug', action='store_true')

connection_args = parser.add_argument_group(u"Соединение с багрепортом YouTrack")
connection_args.add_argument("-a", "--address", type=str,
                             help=u"Адрес багрепорта",
                             dest="address", required=True)
connection_args.add_argument("-u", "--user", type=str,
                             help=u"Имя пользователя YouTrack, от имени которого создаются задачи",
                             dest="username", required=True)
connection_args.add_argument("-s", "--secret", type=argparse.FileType('r'),
                             help=u"Имя текстового файла, который содержит пароль пользователя YouTrack",
                             dest="password", required=True)
connection_args.add_argument("-r", "--project", type=str,
                             help=u"Имя проекта, в который будут добавляться задачи",
                             dest="project", required=True)
source_args = parser.add_argument_group(u"Исходные данные")
source_args.add_argument("-i", "--input", type=argparse.FileType('r'),
                         help=u"Имя входного файла",
                         dest="inputfile", default="report.txt")
source_args.add_argument("-w", "--workdir", type=str,
                         help=u"Корневой каталог с файлами тестов",
                         dest="root", default="..")
misc_args = parser.add_argument_group(u"Оформление")
misc_args.add_argument("-t", "--title", type=unicode,
                       help=u"Заголовок задач в багрепорте",
                       dest="summary",
                       default=u"Не проходят тесты")

args = parser.parse_args()


logging.getLogger().name = "report2youtrack.py"

if args.debug:
    logging.getLogger().setLevel(logging.DEBUG)
else:
    logging.getLogger().setLevel(logging.INFO)

address = args.address
username = args.username
project_name = args.project
inputfile = args.inputfile
root = args.root
summary = args.summary

# if not os.path.exists(password):
#     logging.error(u"Файл с паролем не существует: %s" % password)
#     sys.exit(1)

password = args.password.read().strip()


class Report:
    def __init__(self, blob):
        self.text = blob.decode('utf-8')
        self.comments = []
        self._load_referred_files()

    def is_empty(self):
        return not self.references

    def is_greater_then(self, another):
        if another:
            assert isinstance(another, Report)
            my_keys = set(self.references.keys())
            another_keys = set(another.references.keys())
            return my_keys > another_keys
        else:
            return True

    def add_comment(self, comment):
        self.comments += [comment]
        self._load_referred_files()

    def _load_referred_files(self):
        alist = self._extract_kumfile_names(self.text + "\n" + reduce(lambda x, y: x + "\n" + y, self.comments, ""))
        self.references = {}
        for name in alist:
            path = self._find_full_path(root, name)
            if path:
                f = open(path, 'r')
                data = f.read()
                f.close()
                self.references[name] = data

    def _extract_kumfile_names(self, text):
        result = []
        lines = text.split('\n')
        for line in lines:
            m = re.search(r"[0-9a-zA-Z_\-]+\.(kum|kod)", line)
            if m:
                kumname = m.group(0)
                if kumname[-3:] == "kod":
                    kumname = kumname[:-3] + "kum"
                if not kumname in result:
                    result += [kumname]
        return result

    def _find_full_path(self, dirname, filename_to_find):
        entries = os.listdir(dirname)
        result = None
        for entry in entries:
            if result:
                break
            if os.path.isdir(dirname + "/" + entry):
                result = self._find_full_path(dirname + "/" + entry, filename_to_find)
            elif entry == filename_to_find:
                result = dirname + "/" + entry
        return result

    def description(self):
        return self.text.encode('utf-8')

    def get_attachments(self):
        return [(name.encode('utf-8'), data) for name, data in self.references.items()]

    def debug(self):
        return u"\n\nОтчет:\n" + self.text + "\n" + \
            u"Комментарии:\n" + string.join(self.comments, "\n") + "\n" + \
            u"Список файлов (" + str(len(self.references.keys())) + "): " + \
               string.join(self.references.keys(), ", ") + "\n\n"


class RESTConection:
    def __init__(self, baseUrl):
        self.baseUrl = baseUrl
        self.cookie = ""

    def GET(self, relativeUrl, parameters={}):
        headers = {}
        if self.cookie:
            headers["Cookie"] = self.cookie
        request = urllib2.Request(self._make_url(relativeUrl, parameters), None, headers)
        request.get_method = lambda: 'GET'
        return self._process_request(request)

    def POST(self, relativeUrl, parameters={}, data=None, contentType=""):
        headers = {}
        if self.cookie:
            headers["Cookie"] = self.cookie
        if contentType:
            headers["Content-Type"] = contentType
        request = urllib2.Request(self._make_url(relativeUrl, parameters), data, headers)
        request.get_method = lambda: 'POST'
        return self._process_request(request)

    def PUT(self, relativeUrl, parameters={}, data=None, contentType=""):
        headers = {}
        if self.cookie:
            headers["Cookie"] = self.cookie
        if contentType:
            headers["Content-Type"] = contentType
        request = urllib2.Request(self._make_url(relativeUrl, parameters), data, headers)
        request.get_method = lambda: 'PUT'
        return self._process_request(request)

    def _make_url(self, relativeUrl, parameters):
        url = self.baseUrl
        if url[-1] != '/' and relativeUrl[0] != '/':
            url += '/'
        url += relativeUrl
        if parameters:
            url += '?' + urllib.urlencode(parameters)
        return url

    def _process_request(self, request):
        assert isinstance(request, urllib2.Request)
        url = request.get_full_url()
        m = re.search(r"password=(.+)", url)
        if m:
            start = m.start()
            end = m.end()
            url = url[:start] + "password=*************" + url[end:]
        logging.debug(u"Запрос HTTP: %s %s" % (request.get_method(), url))
        try:
            response = urllib2.urlopen(request)
        except urllib2.HTTPError as error:
            logging.error("HTTPError %d %s while access to %s with headers %s" % (
                error.code, error.msg, url, str(request.headers)
            ))
            response = None
        if response:
            headers = response.headers.dict
            if 'set-cookie' in headers:
                self.cookie = headers['set-cookie']
            data = response.read()
            code = int(response.code)
            logging.debug(u"Ответ HTTP %d:\n%s\n" % (code, unicode(data, 'utf-8')))
            if response.code == 201:
                return headers['location'][len(self.baseUrl):]
            assert isinstance(data, str)
            if data.strip().startswith("<"):
                return xml.etree.ElementTree.fromstring(data)
            else:
                return data
        else:
            return None


base_url = address
if base_url[-1] != '/':
    base_url += '/'
base_url += 'rest'

connection = RESTConection(base_url)
logging.debug(u"Попытка авторизации пользователя %s" % username)
login = connection.POST("/user/login", {"login": username, "password": password})
if login is None or login.text != "ok":
    logging.error(u"Не могу авторизоваться в YouTrack")
    sys.exit(1)

logging.debug(u"Успешная авторизация в YouTrack")


def get_last_report():
    logging.debug(u"Поиск в багрепорте уже существующей задачи")
    my_issues = connection.GET("/issue/byproject/" + project_name, {
        "filter": "from:%s state:-fixed" % username,
        "max": 1
    })
    issue = my_issues.find("issue")
    result = None
    if issue is not None:
        fields = issue.findall("field")
        for value in fields:
            assert isinstance(value, xml.etree.ElementTree.Element)
            name = value.attrib["name"]
            if name == "description":
                description = value.findtext("value")
                result = Report(description.encode('utf-8'))
                break
        comments = issue.findall("comment")
        if comments:
            for value in comments:
                assert isinstance(value, xml.etree.ElementTree.Element)
                deleted = value.attrib["deleted"]
                text = value.attrib["text"]
                if deleted != "true":
                    result.add_comment(text)
        result.id = issue.attrib["id"]
    return result

new_report = Report(args.inputfile.read())
logging.debug(u"Загружен файл отчета:\n %s" % new_report.debug())

old_report = get_last_report()
if old_report:
    logging.debug(u"На багрепорте уже имеется задача %s:\n%s" % (old_report.id, old_report.debug()))


def create_new_issue(report):
    assert isinstance(report, Report)
    logging.debug(u"Запрос на создание новой задачи")
    issue = connection.PUT("/issue", {
        "project": project_name,
        "summary": summary.encode("utf-8")},
        "description=" + report.description()
    )
    Boundary = "---------------------------7d159c1302d0y0"
    NL = "\r\n"
    message = ""
    names = []
    for name, data in report.get_attachments():
        names += [name]
        message += "--" + Boundary + NL
        message += "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"" % (name, name) + NL
        message += "Content-Type: text-plain" + NL
        message += "Content-Transfer-Encoding: binary" + NL
        message += NL
        message += data + NL
    message += "--" + Boundary + "--" + NL
    message += NL
    logging.debug(u"Запрос на добавление вложений %s" % string.join(names, ", "))
    connection.POST(issue + "/attachment", {}, message, "multipart/form-data; boundary=%s" % Boundary)


def update_issue(old_report, new_report):
    assert isinstance(old_report, Report)
    assert isinstance(new_report, Report)
    comment = new_report.description()
    logging.debug(u"Запрос на добавление комментария к задаче")
    connection.POST("/issue/%s/execute" % old_report.id, {}, "comment="+comment)
    logging.debug(u"Запрос списка вложений к задаче")
    file_urls_elem = connection.GET("/issue/%s/attachment" % old_report.id)
    file_urls = file_urls_elem.findall("fileUrl")
    existing_files = []
    for file_url in file_urls:
        name = file_url.attrib["name"]
        if not name in existing_files:
            existing_files += [name]
    Boundary = "---------------------------7d159c1302d0y0"
    NL = "\r\n"
    message = ""
    for name, data in new_report.get_attachments():
        if name in existing_files:
            continue
        message += "--" + Boundary + NL
        message += "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"" % (name, name) + NL
        message += "Content-Type: text-plain" + NL
        message += "Content-Transfer-Encoding: binary" + NL
        message += NL
        message += data + NL
    if message:
        message += "--" + Boundary + "--" + NL
        message += NL
        logging.debug(u"Запрос на добавление вложения %s" % name)
        connection.POST("/issue/" + old_report.id + "/attachment", {}, message,
                        "multipart/form-data; boundary=%s" % Boundary)


if new_report.is_empty():
    logging.info(u"Все тесты пройдены успешно, задачи в багрепорте не изменены")
elif not new_report.is_greater_then(old_report):
    logging.info(u"Отчет об ошибках уже имеется в багрепорте либо одном из его комментариев")
elif not old_report:
    create_new_issue(new_report)
    logging.info(u"Создана новая задача в багрепорте")
else:
    update_issue(old_report, new_report)
    logging.info(u"Создан комментарий к существующей задаче в багрепорте")








