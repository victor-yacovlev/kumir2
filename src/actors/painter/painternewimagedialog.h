#ifndef PAINTERNEWIMAGEDIALOG_H
#define PAINTERNEWIMAGEDIALOG_H

#include <QtGui>

namespace ActorPainter {

namespace Ui {
    class PainterNewImageDialog;
}

class PainterNewImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PainterNewImageDialog(QWidget *parent = 0);
    ~PainterNewImageDialog();
    int w() const;
    int h() const;
    QString color() const;
    QString templateFileName() const;
    QString templateName() const;
    bool isTemplate() const;
public slots:
    int exec();
private slots:
    void handleColorChanged(const QString &text);
    void handleTemplateClicked(QListWidgetItem * item);
private:
    QString s_color;
    Ui::PainterNewImageDialog *ui;
    QStringList sl_templateFiles;
    QStringList sl_templateNames;
};

}

#endif // PAINTERNEWIMAGEDIALOG_H
