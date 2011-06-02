#include "painterwindow.h"
#include "ui_painterwindow.h"
#include "painternewimagedialog.h"
#include "paintertools.h"



namespace ActorPainter {

PainterWindow::PainterWindow(QSettings * settings, QWidget *parent) :
    VisualComponent(),
    ui(new Ui::PainterWindow),
    m_settings(settings)
{
    setParent(parent);
    setMinimumSize(400, 300);
    ui->setupUi(this);
    ui->horizontalRuler->setScrollBar(ui->scrollArea->horizontalScrollBar());
    ui->verticalRuler->setScrollBar(ui->scrollArea->verticalScrollBar());
    ui->horizontalRuler->setView(ui->view);
    ui->verticalRuler->setView(ui->view);
    connect (ui->scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),
             ui->horizontalRuler, SLOT(update()));
    connect (ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
             ui->verticalRuler, SLOT(update()));
    connect (ui->view, SIGNAL(cursorOver(int,int,QColor)), this, SLOT(handleCursorMoved(int,int,QColor)));
    m_newImageDialog = new PainterNewImageDialog(this);
    connect (ui->actionNew, SIGNAL(triggered()), this, SLOT(newImage()));
    connect (ui->actionLoad_image, SIGNAL(triggered()), this, SLOT(loadImage()));
    connect (ui->actionSave_image, SIGNAL(triggered()), this, SLOT(saveImage()));
    connect (ui->actionSave_image_as, SIGNAL(triggered()), this, SLOT(saveImageAs()));
    connect (ui->actionReset, SIGNAL(triggered()), this, SLOT(reset()));

    connect (ui->actionCMYK, SIGNAL(triggered()), this, SLOT(handleColorTextModeChanged()));
    connect (ui->actionRGB, SIGNAL(triggered()), this, SLOT(handleColorTextModeChanged()));
    connect (ui->actionHSL, SIGNAL(triggered()), this, SLOT(handleColorTextModeChanged()));
    connect (ui->actionHSV, SIGNAL(triggered()), this, SLOT(handleColorTextModeChanged()));
    connect (ui->actionHTML, SIGNAL(triggered()), this, SLOT(handleColorTextModeChanged()));

    QSettings s;
    restoreGeometry(s.value("Plugins/Painter/WindowGeometry").toByteArray());
    qreal initialZoom = 1.0;
    QRect screenRect = qApp->desktop()->availableGeometry();
    if (screenRect.height()<700)
        initialZoom = 0.5;
    initialZoom = s.value("Plugins/Painter/ViewZoom",initialZoom).toReal();
    e_showColorMode = s.value("Plugins/Painter/ShowColorMode","HTML").toString();
    if (e_showColorMode=="HTML")
        ui->actionHTML->setChecked(true);
    if (e_showColorMode=="RGB")
        ui->actionRGB->setChecked(true);
    if (e_showColorMode=="CMYK")
        ui->actionCMYK->setChecked(true);
    if (e_showColorMode=="HSL")
        ui->actionHSL->setChecked(true);
    if (e_showColorMode=="HSV")
        ui->actionHSV->setChecked(true);
    ui->view->setZoom(initialZoom);
    ui->horizontalRuler->setZoom(initialZoom);
    ui->verticalRuler->setZoom(initialZoom);


    connect(ui->actionOriginal_size, SIGNAL(triggered()),
            this, SLOT(handleScale()));
    connect(ui->actionHalf_size, SIGNAL(triggered()),
            this, SLOT(handleScale()));
    connect(ui->actionFit_width, SIGNAL(triggered()),
            this, SLOT(handleScale()));
    connect(ui->actionFit_height, SIGNAL(triggered()),
            this, SLOT(handleScale()));
    connect(ui->actionFit_both, SIGNAL(triggered()),
            this, SLOT(handleScale()));

    handleCursorMoved(-1,-1,"");
    ui->zoomLabel->setText(QString::number(int(ui->view->zoom()*100))+"%");



}

void PainterWindow::setCanvas(QImage * canvas, QMutex *locker)
{
    ui->view->setCanvas(canvas, locker);
    ui->horizontalRuler->setCanvas(canvas);
    ui->verticalRuler->setCanvas(canvas);
}

QWidget * PainterWindow::view()
{
    return ui->view;
}

void PainterWindow::handleCursorMoved(int x, int y, const QColor &color)
{
    const QString colorRgb = QString("rgb(%1,%2,%3)")
                             .arg(color.red())
                             .arg(color.green())
                             .arg(color.blue());
    const QString colorHumanName = CSS_RGB_toString(colorRgb);
    QString colorName;
    if (e_showColorMode=="RGB") {
        colorName = QString("%1, %2, %3")
                    .arg(color.red())
                    .arg(color.green())
                    .arg(color.blue());
    }
    else if (e_showColorMode=="CMYK") {
        colorName = QString("%1, %2, %3, %4")
                    .arg(color.cyan())
                    .arg(color.magenta())
                    .arg(color.yellow())
                    .arg(color.black());
    }
    else if (e_showColorMode=="HSL") {
        colorName = QString("%1, %2, %3")
                    .arg(color.hue())
                    .arg(color.saturation())
                    .arg(color.lightness());
    }
    else if (e_showColorMode=="HSV") {
        colorName = QString("%1, %2, %3")
                    .arg(color.hue())
                    .arg(color.saturation())
                    .arg(color.value());
    }
    else {
        colorName = color.name().toUpper();
    }
    if (!colorHumanName.isEmpty()) {
        colorName += " ("+colorHumanName+")";
    }
    if (x==-1 || y==-1) {
        ui->horizontalRuler->highlightValue(-1);
        ui->verticalRuler->highlightValue(-1);
        ui->statusBar->setText("");
    }
    else {
        ui->horizontalRuler->highlightValue(x);
        ui->verticalRuler->highlightValue(y);
        ui->statusBar->setText(tr("Coordinates: X = %1, Y = %2; Colour: %3").arg(x).arg(y).arg(colorName));
    }
}

void PainterWindow::handleColorTextModeChanged()
{
    QAction *a = qobject_cast<QAction*>(sender());
    QList<QAction*> as;
    as << ui->actionCMYK;
    as << ui->actionHSL;
    as << ui->actionHSV;
    as << ui->actionRGB;
    as << ui->actionHTML;
    as.removeAll(a);
    a->setChecked(true);
    foreach (QAction *aa, as) {
        aa->setChecked(false);
    }
    if (a==ui->actionCMYK)
        e_showColorMode = "CMYK";
    if (a==ui->actionRGB)
        e_showColorMode = "RGB";
    if (a==ui->actionHSL)
        e_showColorMode = "HSL";
    if (a==ui->actionHSV)
        e_showColorMode = "HSV";
    if (a==ui->actionHTML)
        e_showColorMode = "HTML";
}

void PainterWindow::newImage()
{
    if (m_newImageDialog->exec()==QDialog::Accepted) {
        int w = m_newImageDialog->w();
        int h = m_newImageDialog->h();
        s_fileName = "";
        s_templateName = m_newImageDialog->templateFileName();
        emit newImageRequest(w, h, m_newImageDialog->color(), m_newImageDialog->isTemplate(), s_templateName);
        if (m_newImageDialog->isTemplate()) {
            setWindowTitle(tr("Painter [%1]").arg(m_newImageDialog->templateName()));
        }
        else {
            setWindowTitle(tr("Painter [новый лист]"));
        }
    }
}

void PainterWindow::loadImage()
{
    QString searchDir = QDir::homePath();
    QSettings s;
    searchDir = s.value("Plugins/Painter/LastDir", searchDir).toString();
    QString fn = QFileDialog::getOpenFileName(this,
                                              tr("Load image..."),
                                              searchDir,
                                              tr("Images (*.png)"));
    if (!fn.isEmpty() && QFile::exists(fn)) {
        emit loadImageRequest(fn);
        s_templateName = fn;
        s.setValue("Plugins/Painter/LastDir", QFileInfo(fn).dir().absolutePath());
        setWindowTitle(tr("Painter [%1]").arg(QFileInfo(fn).fileName()));
    }
}

void PainterWindow::saveImage()
{
    if (s_fileName.isEmpty()) {
        saveImageAs();
    }
    else {
        saveImageToFile(s_fileName);
    }
}

void PainterWindow::saveImageAs()
{
    QString searchDir = QDir::homePath();
    QSettings s;
    QString suggestName = s_templateName.isEmpty()?
                          QString::fromUtf8("picture.png") :
                          QString::fromUtf8("copy of ")+QFileInfo(s_templateName).fileName();
    searchDir = s.value("Plugins/Painter/LastDir", searchDir).toString();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save image..."),
                                searchDir+"/"+suggestName,
                                tr("Images (*.png)"));
    if (!fileName.isEmpty()) {
        saveImageToFile(fileName);
        s_fileName = fileName;
        if (s_templateName.isEmpty()) {
            setWindowTitle(tr("%1 - Painter").arg(QFileInfo(fileName).fileName()));
        }
        else {
            setWindowTitle(tr("%2 - Painter [%1]").arg(QFileInfo(s_templateName).fileName()).arg(QFileInfo(fileName).fileName()));
        }
        s.setValue("Plugins/Painter/LastDir", QFileInfo(fileName).dir().absolutePath());
    }
}

void PainterWindow::saveImageToFile(const QString &fileName)
{
    ui->view->canvas()->save(fileName, "PNG");
}

void PainterWindow::reset()
{
    emit resetRequest();
}

void PainterWindow::closeEvent(QCloseEvent *event)
{
    m_settings->setValue("Plugins/Painter/WindowGeometry",saveGeometry());
    m_settings->setValue("Plugins/Painter/ViewZoom", ui->view->zoom());
    m_settings->setValue("Plugins/Painter/ShowColorMode",e_showColorMode);
    VisualComponent::closeEvent(event);
}

void PainterWindow::hideEvent(QHideEvent *event)
{
    QSettings s;
    s.setValue("Plugins/Painter/WindowGeometry",saveGeometry());
    s.setValue("Plugins/Painter/ViewZoom", ui->view->zoom());
    s.setValue("Plugins/Painter/ShowColorMode",e_showColorMode);
    VisualComponent::hideEvent(event);
}

void PainterWindow::handleScale()
{
    qreal scale = 1.0;
    QAction *act = qobject_cast<QAction*>(sender());
    if (act==ui->actionHalf_size)
        scale = 0.5;
    else if (act==ui->actionFit_width) {
        qreal imW = ui->view->canvas()->width()+60;
        qreal wW = ui->scrollArea->maximumViewportSize().width();
        scale = wW/imW;
    }
    else if (act==ui->actionFit_height) {
        qreal imH = ui->view->canvas()->height()+60;
        qreal wH = ui->scrollArea->maximumViewportSize().height();
        scale = wH/imH;
    }
    else if (act==ui->actionFit_both) {
        qreal imH = ui->view->canvas()->height()+60;
        qreal wH = ui->scrollArea->maximumViewportSize().height();
        qreal scaleH = wH/imH;
        qreal imW = ui->view->canvas()->width()+60;
        qreal wW = ui->scrollArea->maximumViewportSize().width();
        qreal scaleW = wW/imW;
        scale = qMin(scaleW, scaleH);
    }
    ui->view->setZoom(scale);
    ui->verticalRuler->setZoom(scale);
    ui->horizontalRuler->setZoom(scale);
    ui->zoomLabel->setText(QString::number(int(ui->view->zoom()*100))+"%");
}


void PainterWindow::handleFullScreen()
{
//    if (qApp->desktop()->screenCount()>1) {

//    }
//    ui->view->showFullScreen();
}

PainterWindow::~PainterWindow()
{
    delete ui;
}

QList<MenuActionsGroup> PainterWindow::menuActions()
{
    MenuActionsGroup page;
    MenuActionsGroup zoom;
    MenuActionsGroup sbar;

    page.menuText = tr("Page");
    zoom.menuText = tr("Zoom");
    sbar.menuText = tr("Statusbar colour representation");

    QAction * sep = new QAction(this);
    sep->setSeparator(true);

    page.actions << ui->actionNew;
    page.actions << ui->actionLoad_image;
    page.actions << sep;
    page.actions << ui->actionReset;
    page.actions << sep;
    page.actions << ui->actionSave_image;
    page.actions << ui->actionSave_image_as;

    zoom.actions << ui->actionHalf_size;
    zoom.actions << ui->actionOriginal_size;
    zoom.actions << sep;
    zoom.actions << ui->actionFit_width;
    zoom.actions << ui->actionFit_height;
    zoom.actions << ui->actionFit_both;

    sbar.actions << ui->actionHTML;
    sbar.actions << ui->actionRGB;
    sbar.actions << ui->actionCMYK;
    sbar.actions << ui->actionHSL;
    sbar.actions << ui->actionHSV;

    return QList<MenuActionsGroup>() << page << zoom << sbar;
}

QList<QAction*> PainterWindow::toolbarActions()
{
    return QList<QAction*>();
}

}

