#ifndef ASYNCHRONOUSGAQTWIDGET_H
#define ASYNCHRONOUSGAQTWIDGET_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTimer>
#include <QDateTime>
#include <QProcess>

#include <sstream>
#include <iostream>

namespace Ui {
class AsynchronousGAQtWidget;
}

// simple guard class for std::cerr stream capture
class cerrRedirect
{
public:
    cerrRedirect(std::streambuf *newBuffer)
    {
        oldBuffer = std::cerr.rdbuf(newBuffer);
    }
    ~cerrRedirect()
    {
        std::cerr.rdbuf(oldBuffer);
    }
private:
    std::streambuf *oldBuffer;
};

class AsynchronousGAQtWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit AsynchronousGAQtWidget(QWidget *parent = nullptr);
    virtual ~AsynchronousGAQtWidget() override;

public slots:
    void pushButtonParameterFileClicked();
    void pushButtonStartingPopulationFileClicked();
    void pushButtonXMLMasterFileClicked();
    void pushButtonOutputFolderClicked();
    void pushButtonModelConfigurationFileClicked();
    void pushButtonModelPopulationFileClicked();
    void pushButtonDriverFileClicked();
    void pushButtonWorkingFolderClicked();
    void pushButtonMergeXMLFileClicked();
    void pushButtonGaitSymExecutableClicked();
    void pushButtonGAExecutableClicked();
    void pushButtonStartClicked();
    void pushButtonStopClicked();
    void checkBoxMergeXMLActivateClicked();
    void handleFinished();
    void lineEditOutputFolderTextChanged(const QString &text);
    void lineEditParameterFileTextChanged(const QString &text);
    void lineEditXMLMasterFileTextChanged(const QString &text);
    void lineEditStartingPopulationFileTextChanged(const QString &text);
    void lineEditModelConfigurationFileTextChanged(const QString &text);
    void lineEditModelPopulationFileTextChanged(const QString &text);
    void lineEditDriverFileTextChanged(const QString &text);
    void lineEditMergeXMLFileTextChanged(const QString &text);
    void lineEditWorkingFolderTextChanged(const QString &text);
    void lineEditGaitSymExecutableTextChanged(const QString &text);
    void lineEditGAExecutableTextChanged(const QString &text);
    void checkBoxStateChanged(int state);
    void spinBoxLogLevelTextChanged(const QString &text);
    void spinBoxTextChanged(const QString &text);
    void appendProgress(const QString &text);
    void setProgressValue(int value);
    void setResultNumber(int number);
    void setBestScore(double value);
    void setEvolveID(uint64_t value);
    void tryToStopGA();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void editSettings();
//    void print();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void font();
//    void bold();
//    void italic();
//    void leftAlign();
//    void rightAlign();
//    void justify();
//    void center();
//    void setLineSpacing();
//    void setParagraphSpacing();
    void about();
//    void aboutQt();

    void runMergeXML();
    void menuRequestPath(QPoint pos);

    void readStandardError();
    void readStandardOutput();

private:
    void closeEvent(QCloseEvent *event) override;
    void setCustomTitle();
    void activateButtons();


    Ui::AsynchronousGAQtWidget *ui = nullptr;
    QProcess *m_ga = nullptr;
    QTimer *m_mergeXMLTimer = nullptr;
    int m_runMergeXML = 0;
    double m_currentLoopValue = 0;
    int m_currentLoopCount = 0;
    QDateTime m_lastResultsTime;
    int m_lastResultsNumber = -1;

    bool m_asynchronousGAFileModified = false;
    QString m_asynchronousGAFileName;
    bool m_asynchronousGAFileNameValid = false;

    std::string m_outputFolder;
    std::string m_parameterFile;
    std::string m_xmlMasterFile;
    std::string m_startingPopulationFile;

    void createActions();
    void createMenus();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void runGA();
    void runGaitSym();
    void open(const QString &fileName);
    void setEditorFonts();

    std::stringstream m_capturedCerr;
    std::unique_ptr<cerrRedirect> m_redirect;

    QMenu *fileMenu;
    QMenu *editMenu;
//    QMenu *formatMenu;
    QMenu *helpMenu;
//    QActionGroup *alignmentGroup;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *editSettingsAct;
//    QAction *printAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *fontAct;
//    QAction *boldAct;
//    QAction *italicAct;
//    QAction *leftAlignAct;
//    QAction *rightAlignAct;
//    QAction *justifyAct;
//    QAction *centerAct;
//    QAction *setLineSpacingAct;
//    QAction *setParagraphSpacingAct;
    QAction *aboutAct;
//    QAction *aboutQtAct;

    QFont m_editorFont;
    QBasicTimer m_basicTimer;
};

#endif // ASYNCHRONOUSGAQTWIDGET_H
