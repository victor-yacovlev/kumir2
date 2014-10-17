#include "remotecontrol.h"
#include <QSvgRenderer>
#include <QDebug>
#include <QPointF>
#include <QPainter>
#include <QClipboard>

#include "extensionsystem/pluginmanager.h"
#include "interfaces/analizerinterface.h"
#include "interfaces/actorinterface.h"

#include "isometricrobotmodule.h"

namespace ActorIsometricRobot {

static const int LoggerPadding = 3;

SvgRemoteControl::SvgRemoteControl(ExtensionSystem::KPlugin * plugin,
                                   IsometricRobotModule * module,
                                   const QString & rcFileName, QWidget *parent)
    : QSvgWidget(rcFileName, parent)
    , plugin_(plugin)
    , module_(module)
    , linkEnabled_(true)
    , loggerOffset_(0)
{
    setupButtons();
    setupLabels();
    setMouseTracking(true);
    QObject::connect(this, SIGNAL(buttonPressed(QString)),
                     this, SLOT(handleSvgButtonPressed(QString)),
                     Qt::QueuedConnection);
    setLinkEnabled(true);
}

void SvgRemoteControl::handleSvgButtonPressed(const QString &svgId)
{
    const QStringList ActionsIds = QStringList()
            << "btn_left" << "btn_right" << "btn_paint" << "btn_forward"
            << "btn_colored" << "btn_clean" << "btn_free" << "btn_wall"
            ;

    if (ActionsIds.contains(svgId)) {
        appendCommandToLog(svgId);
        update();
    }
    else if ("btn_scroll_up" == svgId) {
        if (btnScrollUpEnabled()) {
            loggerOffset_ --;
            update();
        }
    }
    else if ("btn_scroll_down" == svgId) {
        if (btnScrollDownEnabled()) {
            loggerOffset_ ++;
            update();
        }
    }
    else if ("btn_clear" == svgId) {
        loggerOffset_ = 0;
        loggerText_.clear();
        module_->reset();
        update();
    }
    else if ("btn_copy" == svgId) {
        QStringList text;
        for (int i=0; i<loggerText_.size(); i++) {
            text.append(loggerText_[i].first);
        }
        QClipboard * clp = QApplication::clipboard();
        clp->setText(text.join("\n"));
    }

    if ("btn_left" == svgId) {
        module_->runTurnLeft();
        appendStatusToLog();
    }
    else if ("btn_right" == svgId) {
        module_->runTurnRight();
        appendStatusToLog();
    }
    else if ("btn_paint" == svgId) {
        module_->runDoPaint();
        appendStatusToLog();
    }
    else if ("btn_forward" == svgId) {
        module_->runGoForward();
        appendStatusToLog();
    }
    else if ("btn_colored" == svgId) {
        appendResultToLog(module_->runIsCellPainted());
    }
    else if ("btn_clean" == svgId) {
        appendResultToLog(module_->runIsCellClean());
    }
    else if ("btn_free" == svgId) {
        appendResultToLog(module_->runIsFreeAhead());
    }
    else if ("btn_wall" == svgId) {
        appendResultToLog(module_->runIsWallAhead());
    }
}

void SvgRemoteControl::appendCommandToLog(const QString &svgId)
{
    TextLine line;
    line.first = commandNameBySvgId(svgId);
    loggerText_.append(line);
    if (btnScrollDownEnabled()) {
        loggerOffset_ ++;
    }
}

void SvgRemoteControl::appendStatusToLog()
{
    using namespace Shared;
    ActorInterface * actor = qobject_cast<ActorInterface*>(plugin_);
    bool error = actor->errorText().length() > 0;
    const QString status = error? tr("Error") : tr("OK");
    loggerText_.last().second = status;
    update();
}

void SvgRemoteControl::appendResultToLog(bool result)
{
    using namespace Shared;
    using namespace ExtensionSystem;

    AnalizerInterface* analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();

    QString msg;

    if (analizer && "python" == analizer->languageName().toLower()) {
        msg = result ? "True" : "False";
    }
    else if (analizer && "pascal" == analizer->languageName().toLower()) {
        msg = result ? "true" : "false";
    }
    else {
        msg = result ? QString::fromUtf8("да") : QString::fromUtf8("нет");
    }

    loggerText_.last().second = msg;
}

QString SvgRemoteControl::commandNameBySvgId(const QString &svgId) const
{
    using namespace Shared;
    using namespace ExtensionSystem;

    QMap<QString,QString> matching;

    AnalizerInterface* analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();

    if (analizer && analizer->primaryAlphabetIsLatin()) {
        matching["btn_left"] = "turn left";
        matching["btn_right"] = "turn right";
        matching["btn_paint"] = "do paint";
        matching["btn_forward"] = "go forward";
        matching["btn_colored"] = "is cell painted";
        matching["btn_clean"] = "is cell clean";
        matching["btn_free"] = "is free ahead";
        matching["btn_wall"] = "is wall ahead";
    }
    else {
        matching["btn_left"] = QString::fromUtf8("повернуть налево");
        matching["btn_right"] = QString::fromUtf8("повернуть направо");
        matching["btn_paint"] = QString::fromUtf8("закрасить");
        matching["btn_forward"] = QString::fromUtf8("вперед");
        matching["btn_colored"] = QString::fromUtf8("клетка закрашена");
        matching["btn_clean"] = QString::fromUtf8("клетка чистая");
        matching["btn_free"] = QString::fromUtf8("впереди свободно");
        matching["btn_wall"] = QString::fromUtf8("впереди стена");
    }

    QString result = matching[svgId];

    if (analizer && QString::fromUtf8("кумир")==analizer->languageName().toLower()) {
    }
    else if (analizer && analizer->caseInsensitiveGrammatic()) {
        result.replace(' ', "");
    }
    else {
        result.replace(' ', "_");
    }

    return result;
}



void SvgRemoteControl::setupButtons()
{
    static const QStringList Names = QStringList()
            << "btn_left" << "btn_right" << "btn_paint" << "btn_forward"
            << "btn_colored" << "btn_clean" << "btn_free" << "btn_wall"
            << "btn_clear" << "btn_copy" << "btn_scroll_up" << "btn_scroll_down";
               ;
    foreach (const QString & id, Names) {
        if (renderer()->elementExists(id)) {
            const QRectF rect = renderer()->boundsOnElement(id);
            buttons_[rect] = id;
        }
        else {
            qWarning() << "No element " << id << " in SVG file!";
        }
    }
}

void SvgRemoteControl::setupLabels()
{
    if (renderer()->elementExists("label_haslink")) {
        linkOnRect_ = renderer()->boundsOnElement("label_haslink");
    }
    if (renderer()->elementExists("label_nolink")) {
        linkOffRect_ = renderer()->boundsOnElement("label_nolink");
    }
}

QRect SvgRemoteControl::scaleToPixels(const QRectF &points) const
{
    const QSize svgSize = renderer()->defaultSize();
    const QRectF viewBox = renderer()->viewBoxF();
    const QSizeF viewBoxSize = viewBox.size();
    const qreal scaleFactor = svgSize.width() / viewBoxSize.width();
    const QRectF resultF = QRectF(points.left()*scaleFactor,
                                  points.top()*scaleFactor,
                                  points.width()*scaleFactor,
                                  points.height()*scaleFactor);
    const QRect result = resultF.toRect();
    return result;
}

void SvgRemoteControl::mousePressEvent(QMouseEvent *event)
{
    const QString prev = buttonPressId_;
    if (buttonHoverId_.length() > 0) {
        buttonPressId_ = buttonHoverId_;
        event->accept();
    }
    else {
        event->ignore();
    }
    if (prev != buttonPressId_) {
        update();
    }
}

void SvgRemoteControl::mouseReleaseEvent(QMouseEvent *event)
{
    const QString prev = buttonPressId_;
    buttonPressId_ = "";
    event->accept();
    if (prev != buttonPressId_) {
        update();
    }
    if (prev.length() > 0 && buttonHoverId_.length() > 0) {
        emit buttonPressed(prev);
    }
}

void SvgRemoteControl::mouseMoveEvent(QMouseEvent *event)
{
    const QString prev = buttonHoverId_;
    const QPoint where = event->pos();
    buttonHoverId_ = "";
    foreach (const QRectF & rectPoints, buttons_.keys()) {
        const QRect rectPixels = scaleToPixels(rectPoints);
        if (rectPixels.contains(where)) {
            buttonHoverId_ = buttons_[rectPoints];
            break;
        }
    }
    if (prev != buttonHoverId_) {
        update();
    }
    event->accept();
}

void SvgRemoteControl::paintEvent(QPaintEvent *event)
{
    QSvgWidget::paintEvent(event);
    if (buttonHoverId_.length()) {
        QRectF where;
        foreach (const QRectF rect, buttons_.keys()) {
            if (buttons_[rect] == buttonHoverId_) {
                where = rect;
                break;
            }
        }
        bool ignore = false;
        if ("btn_scroll_up" == buttonHoverId_ && !btnScrollUpEnabled())
            ignore = true;
        if ("btn_scroll_down" == buttonHoverId_ && !btnScrollDownEnabled())
            ignore = true;
        const QString hoverId = buttonHoverId_ + "_hover";
        if (renderer()->elementExists(hoverId)) {
            if (!ignore) {
                QPainter painter(this);
                renderer()->render(&painter, hoverId, scaleToPixels(where));
            }
        }
        else {
            qWarning() << "Element not exists in SVG: " << hoverId;
        }
    }
    if (buttonPressId_.length()) {
        QRectF where;
        foreach (const QRectF rect, buttons_.keys()) {
            if (buttons_[rect] == buttonPressId_) {
                where = rect;
                break;
            }
        }
        const QString pressId = buttonPressId_ + "_press";
        if (renderer()->elementExists(pressId)) {
            QPainter painter(this);
            renderer()->render(&painter, pressId, scaleToPixels(where));
        }
        else {
            qWarning() << "Element not exists in SVG: " << pressId;
        }
    }
    if (renderer()->elementExists("label_haslink") && renderer()->elementExists("label_nolink")) {
        if (linkEnabled_ && renderer()->elementExists("label_haslink_active")) {
            QPainter painter(this);
            renderer()->render(&painter, "label_haslink_active", scaleToPixels(linkOnRect_));
        }
        else if (renderer()->elementExists("label_nolink_active")) {
            QPainter painter(this);
            renderer()->render(&painter, "label_nolink_active", scaleToPixels(linkOffRect_));
        }
    }
    if (renderer()->elementExists("widget_logger")) {
        const QRect loggerRect = scaleToPixels(renderer()->boundsOnElement("widget_logger"));
        QPainter painter(this);
        paintLogger(&painter, loggerRect);
    }
}

void SvgRemoteControl::paintLogger(QPainter *painter, const QRect &rect)
{

    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setPen(Qt::black);
    const QFontMetrics fm = painter->fontMetrics();
    const int lineHeight = fm.lineSpacing();
    const int visibleLinesCount = visibleLoggerLinesCount();
    for (int visLineNo = 0; visLineNo < visibleLinesCount; visLineNo ++) {
        const int realLineNo = visLineNo + loggerOffset_;
        if (0 <= realLineNo && realLineNo < loggerText_.size()) {
            const int x1 = rect.left() + LoggerPadding;
            const int y = rect.top() + (1 + visLineNo) * lineHeight + LoggerPadding;
            const TextLine & line = loggerText_.at(realLineNo);
            const QString & commandText = line.first;
            const QString & statusText = line.second;
            painter->drawText(x1, y, commandText);
            if (statusText.length() > 0) {
                const int statusWidth = fm.width(statusText);
                const int x2 = rect.right() - LoggerPadding - statusWidth;
                painter->drawText(x2, y, statusText);
            }
        }
    }
}

int SvgRemoteControl::visibleLoggerLinesCount() const
{
    const QRect loggerRect = scaleToPixels(renderer()->boundsOnElement("widget_logger"));
    const QFontMetrics fm = fontMetrics();
    const int lineHeight = fm.lineSpacing();
    const int visibleLinesCount = (loggerRect.height() - 2*LoggerPadding) / lineHeight;
    return visibleLinesCount;
}

bool SvgRemoteControl::btnScrollUpEnabled() const
{
    return loggerOffset_ > 0;
}

bool SvgRemoteControl::btnScrollDownEnabled() const
{
    const int visibleLinesCount = visibleLoggerLinesCount();
    return loggerText_.size() > (visibleLinesCount + loggerOffset_);
}


} // namespace Robot25D
