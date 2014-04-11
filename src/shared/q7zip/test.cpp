#include "q7zip/archive.h"

#include "q7zip/sevenziparchive.h"
#include "q7zip/ziparchive.h"

#include <QtCore>

#include <iostream>

class TestSuite {
public:
    inline explicit TestSuite(Q7Zip::Archive* archive, std::ostream& logStream)
        : archive_(archive), logStream_(logStream) {}

    bool operator()();

    bool listingTest();
    bool simpleTest();
    bool subdirectoryTest();
    bool unicodeTest();
    bool passwordTest();

private:
    Q7Zip::Archive* archive_;
    std::ostream& logStream_;
};

bool TestSuite::operator ()()
{
    bool allOk = true;
    static const char* OK = "passed";
    static const char* FAIL = "failed";

    logStream_ << "Listing: ";
    if (listingTest()) {
        logStream_ << OK;
    }
    else {
        logStream_ << FAIL;
        allOk = false;
    }
    logStream_ << std::endl;

    logStream_ << "Simple file extraction: ";
    if (simpleTest()) {
        logStream_ << OK;
    }
    else {
        logStream_ << FAIL;
        allOk = false;
    }
    logStream_ << std::endl;

    logStream_ << "Subdirectory file extraction: ";
    if (subdirectoryTest()) {
        logStream_ << OK;
    }
    else {
        logStream_ << FAIL;
        allOk = false;
    }
    logStream_ << std::endl;

    logStream_ << "Unicode named file extraction: ";
    if (unicodeTest()) {
        logStream_ << OK;
    }
    else {
        logStream_ << FAIL;
        allOk = false;
    }
    logStream_ << std::endl;

    logStream_ << "Password protected file extraction: ";
    if (passwordTest()) {
        logStream_ << OK;
    }
    else {
        logStream_ << FAIL;
        allOk = false;
    }
    logStream_ << std::endl;

    if (allOk) {
        logStream_ << "All tests passed for this archive" << std::endl << std::endl;
    }
    else {
        logStream_ << "One of tests failed for this archive" << std::endl << std::endl;
    }

    return allOk;
}

bool TestSuite::listingTest()
{
    const QStringList allFiles = archive_->allFilesList();
    return
            allFiles.size() == 4 &&
            allFiles.contains("hello.txt") &&
            allFiles.contains("subdir/hello.txt") &&
            allFiles.contains("withpassword.txt") &&
            allFiles.contains(QString::fromUtf8("привет.txt"));
}

bool TestSuite::simpleTest()
{
    QIODevice * helloTxt = archive_->openFileEntry("hello.txt", QIODevice::ReadOnly);
    if (!helloTxt) return false;
    helloTxt->open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = helloTxt->readAll().simplified();
    helloTxt->close();
    delete helloTxt;
    return QByteArray("Hello, World!") == data;
}

bool TestSuite::subdirectoryTest()
{
    QIODevice * helloTxt = archive_->openFileEntry("subdir/hello.txt", QIODevice::ReadOnly);
    if (!helloTxt) return false;
    helloTxt->open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = helloTxt->readAll().simplified();
    helloTxt->close();
    delete helloTxt;
    return QByteArray("Hello from subdirectory!") == data;
}

bool TestSuite::unicodeTest()
{
    QIODevice * helloTxt = archive_->openFileEntry(QString::fromUtf8("привет.txt"),
                                                  QIODevice::ReadOnly);
    if (!helloTxt) return false;
    helloTxt->open(QIODevice::ReadOnly|QIODevice::Text);
    QString data = QString::fromUtf8(helloTxt->readAll()).simplified();
    helloTxt->close();
    delete helloTxt;
    return QString::fromUtf8("Привет, Мир!") == data;
}

bool TestSuite::passwordTest()
{
    static const QString password = QString::fromAscii("Qwerty123");
    QIODevice * helloTxt = archive_->openFileEntry(QString::fromUtf8("withpassword.txt"),
                                                  QIODevice::ReadOnly,
                                                  password);
    if (!helloTxt) return false;
    helloTxt->open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = helloTxt->readAll().simplified();
    helloTxt->close();
    delete helloTxt;
    return QByteArray("Secure Hello.") == data;
}

int main(int, char **)
{
    bool anyTestSuiteFailed = false;

    QFile archiveFile7z("test.7z");
    Q7Zip::SevenZipArchive * archive7z = new Q7Zip::SevenZipArchive(&archiveFile7z);
    TestSuite test7z(archive7z, std::cout);

    QFile archiveFileZip("test.zip");
    Q7Zip::ZipArchive * archiveZip = new Q7Zip::ZipArchive(&archiveFileZip);
    TestSuite testZip(archiveZip, std::cout);

    std::cout << "=== test.7z ===" << std::endl;
    if (archiveFile7z.open(QIODevice::ReadOnly)) {
        archive7z->open(QIODevice::ReadOnly);
        anyTestSuiteFailed = !test7z() || anyTestSuiteFailed;
        archive7z->close();
        archiveFile7z.close();
    }
    else {
        std::cerr << "Can't open test.7z" << std::endl << std::endl;
        anyTestSuiteFailed = true;
    }

    std::cout << "=== test.zip ===" << std::endl;
    if (archiveFileZip.open(QIODevice::ReadOnly)) {
        archiveZip->open(QIODevice::ReadOnly);
        anyTestSuiteFailed = !testZip() || anyTestSuiteFailed;
        archiveZip->close();
        archiveFileZip.close();
    }
    else {
        std::cerr << "Can't open test.zip" << std::endl << std::endl;
        anyTestSuiteFailed = true;
    }

    delete archive7z;
    delete archiveZip;

    return anyTestSuiteFailed? 1 : 0;
}
