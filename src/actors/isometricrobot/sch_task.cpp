#include "sch_task.h"
#include "fp.h"


namespace Schema {

extern bool parceJSON(const QScriptValue &value, Task &task)
{
    if (value.property("title").isString())
        task.title = value.property("title").toString();
    QScriptValue env = value.property("environment");
    bool envOk = false;
    if (env.isValid()) {
        envOk = parceJSON(env, task.environment);
    }
    QScriptValue hint = value.property("hint");
    if (hint.isObject()) {
        bool compressed = false;
        QString encoding;
        QString mimetype;
        QString strData;
        if (hint.property("compressed").isBool()) {
            compressed = hint.property("compressed").toBool();
        }
        if (hint.property("encoding").isString()) {
            encoding = hint.property("encoding").toString().toLower().trimmed();
        }
        if (hint.property("mimetype").isString()) {
            mimetype = hint.property("mimetype").toString().toLower().trimmed();
        }
        if (hint.property("data").isString()) {
            strData = hint.property("data").toString();
        }
        if (!strData.isEmpty() && !mimetype.isEmpty()) {
            QByteArray decoded;
            if (encoding=="base64") {
                decoded = QByteArray::fromBase64(strData.toLatin1());
            }
            else if (encoding=="hex") {
                decoded = QByteArray::fromHex(strData.toLatin1());
            }
            else {
                decoded = strData.toLocal8Bit();
                compressed = false;
            }
            if (compressed) {
                task.hintData = qUncompress(decoded);
            }
            else {
                task.hintData = decoded;
            }
            QCryptographicHash hash(QCryptographicHash::Md5);
            hash.addData(task.hintData);
            qDebug() << "Load binary data of size " << task.hintData.size() << " with MD5: " << hash.result().toHex();
            task.hintMimeType = mimetype;
        }
    }

    parceJSON(value.property("program"), task.program);
    return envOk;
}



extern QString addLeadingTab(const QString &t)
{
    return "\t"+t;
}

QString screenString(const QByteArray &data)
{
    QString s = QString::fromLocal8Bit(data);
    s.replace("\\", "\\\\");
    s.replace("\n", "\\n");
    s.replace("\"", "\\\"");
    return s;
}

QString encodeData(const QByteArray &data)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(data);
    qDebug() << "Store binary data of size " << data.size() << " with MD5: " << hash.result().toHex();
    QByteArray stage1 = qCompress(data, 9);
    QByteArray stage2 = stage1.toBase64();
    return QString::fromLatin1(stage2);
}

extern QString generateJSON(const Task &task)
{
    const QString template0 = "{\n\t\"title\": %1,\n\t\"hint\": %2,\n\t\"environment\": %3,\n\t\"program\": %4 \n}";
    const QString title = "\""+task.title+"\"";
    QString hint = "null";
    if (!task.hintData.isEmpty() && !task.hintMimeType.isEmpty()) {
        QString mime = task.hintMimeType;
        QString d = mime.startsWith("text/")? screenString(task.hintData) : encodeData(task.hintData);
        bool compressed = !mime.startsWith("text/");
        const QString encoding = mime.startsWith("text/")? "notencoded" : "base64";
        hint = QString::fromLatin1("{ mimetype: \"%1\", encoding: \"%4\", compressed: %2, data: \"%3\" }")
                .arg(mime)
                .arg(compressed? "true" : "false")
                .arg(d)
                .arg(encoding);

    }
    QStringList envLines = generateJSON(task.environment).split("\n");
    for (int j=0; j<envLines.size(); j++) {
        envLines[j] = "\t"+envLines[j];
    }
    const QString env = envLines.join("\n").trimmed();
    QStringList programLines = generateJSON(task.program).split("\n");
    for (int j=0; j<programLines.size(); j++) {
        programLines[j] = "\t"+programLines[j];
    }
    const QString program = programLines.join("\n").trimmed();
    const QString data = template0
            .arg(title)
            .arg(hint)
            .arg(env)
            .arg(program);
    return data;
}

extern bool isEqual(const Task &a, const Task &b)
{
    if (a.title.simplified()!=b.title.simplified())
        return false;
    if (a.hintMimeType.simplified()!=b.hintMimeType.simplified())
        return false;
    if (a.hintData.size()!=b.hintData.size())
        return false;
    if (!isEqual(a.environment, b.environment))
        return false;
    if (!isEqual(a.program, b.program))
        return false;
    for (int i=0; i<a.hintData.size(); i++) {
        char aa = a.hintData[i];
        char bb = b.hintData[i];
        if (aa!=bb)
            return false;
    }
    return true;
}

} // namespace Schema
