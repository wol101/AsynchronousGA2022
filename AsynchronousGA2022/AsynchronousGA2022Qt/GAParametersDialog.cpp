#include "GAParametersDialog.h"
#include "ui_GAParametersDialog.h"

#include "DataFile.h"

#include <QSettings>

#include <vector>
#include <string>

GAParametersDialog::GAParametersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GAParametersDialog)
{
    ui->setupUi(this);

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &GAParametersDialog::reject);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &GAParametersDialog::accept);

    // force a button enable check when tbne combo boxes are altered (since they can have invalid values
    QList<QComboBox *> listQComboBox = this->findChildren<QComboBox *>(QString(), Qt::FindChildrenRecursively);
    for (auto &&it : listQComboBox) { connect(it, &QComboBox::currentIndexChanged, this, &GAParametersDialog::activateButtons); }
    activateButtons();

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "AnimalSimulationLaboratory", "AsynchronousGAQt2019");
    restoreGeometry(settings.value("GAParametersDialogGeometry").toByteArray());
}

GAParametersDialog::~GAParametersDialog()
{
    delete ui;
}

void GAParametersDialog::closeEvent(QCloseEvent *event)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "AnimalSimulationLaboratory", "AsynchronousGAQt2019");
    settings.setValue("GAParametersDialogGeometry", saveGeometry());
    settings.sync();
    QDialog::closeEvent(event);
}

QString GAParametersDialog::editorText() const
{
    QString editorText;
    editorText.append(QString("crossoverType %1\n").arg(ui->comboBoxCrossoverType->currentText()));
    editorText.append(QString("parentSelection %1\n").arg(ui->comboBoxParentSelection->currentText()));
    editorText.append(QString("resizeControl %1\n").arg(ui->comboBoxResizeControl->currentText()));

    editorText.append(QString("bounceMutation %1\n").arg(ui->checkBoxBounceMutation->isChecked()));
    editorText.append(QString("circularMutation %1\n").arg(ui->checkBoxCircularMutation->isChecked()));
    editorText.append(QString("multipleGaussian %1\n").arg(ui->checkBoxMultipleGaussian->isChecked()));
    editorText.append(QString("onlyKeepBestGenome %1\n").arg(ui->checkBoxOnlyKeepBestGenome->isChecked()));
    editorText.append(QString("onlyKeepBestPopulation %1\n").arg(ui->checkBoxOnlyKeepBestPopulation->isChecked()));
    editorText.append(QString("randomiseModel %1\n").arg(ui->checkBoxRandomiseModel->isChecked()));
    editorText.append(QString("minimizeScore %1\n").arg(ui->checkBoxMinimizeScore->isChecked()));

    editorText.append(QString("crossoverChance %1\n").arg(ui->doubleSpinBoxCrossoverChance->value()));
    editorText.append(QString("duplicationMutationChance %1\n").arg(ui->doubleSpinBoxDuplicationMutationChance->value()));
    editorText.append(QString("frameShiftMutationChance %1\n").arg(ui->doubleSpinBoxFrameShiftMutationChance->value()));
    editorText.append(QString("gamma %1\n").arg(ui->doubleSpinBoxGamma->value()));
    editorText.append(QString("gaussianMutationChance %1\n").arg(ui->doubleSpinBoxGaussianMutationChance->value()));
    editorText.append(QString("improvementThreshold %1\n").arg(ui->doubleSpinBoxImprovementThreshold->value()));
    editorText.append(QString("watchDogTimerLimit %1\n").arg(ui->doubleSpinBoxWatchDogTimerLimit->value()));

    editorText.append(QString("genomeLength %1\n").arg(ui->spinBoxGenomeLength->value()));
    editorText.append(QString("improvementReproductions %1\n").arg(ui->spinBoxImprovementReproductions->value()));
    editorText.append(QString("maxReproductions %1\n").arg(ui->spinBoxMaxReproductions->value()));
    editorText.append(QString("outputPopulationSize %1\n").arg(ui->spinBoxOutputPopulationSize->value()));
    editorText.append(QString("outputStatsEvery %1\n").arg(ui->spinBoxOutputStatsEvery->value()));
    editorText.append(QString("parentsToKeep %1\n").arg(ui->spinBoxParentsToKeep->value()));
    editorText.append(QString("populationSize %1\n").arg(ui->spinBoxPopulationSize->value()));
    editorText.append(QString("saveBestEvery %1\n").arg(ui->spinBoxSaveBestEvery->value()));
    editorText.append(QString("savePopEvery %1\n").arg(ui->spinBoxSavePopEvery->value()));

    return editorText;
}

void GAParametersDialog::setEditorText(const QString &newEditorText)
{
    DataFile dataFile;
    std::string decodedData(newEditorText.toStdString());
    dataFile.SetRawData(decodedData.c_str(), decodedData.size());

    static const std::vector<std::string> crossoverType = {"OnePoint", "Average"};
    static const std::vector<std::string> parentSelection = {"UniformSelection", "RankBasedSelection", "SqrtBasedSelection", "GammaBasedSelection"};
    static const std::vector<std::string> resizeControl = {"RandomiseResize", "MutateResize"};

    std::string stringV;
    int index;
    dataFile.RetrieveParameter("crossoverType", &stringV);
    index = -1;
    for (size_t i = 0; i < crossoverType.size(); i++)
    {
        ui->comboBoxCrossoverType->addItem(QString::fromStdString(crossoverType[i]));
        if (crossoverType[i] == stringV) index = int(i);
    }
    ui->comboBoxCrossoverType->setCurrentIndex(index);

    dataFile.RetrieveParameter("resizeControl", &stringV);
    index = -1;
    for (size_t i = 0; i < resizeControl.size(); i++)
    {
        ui->comboBoxResizeControl->addItem(QString::fromStdString(resizeControl[i]));
        if (resizeControl[i] == stringV) index = int(i);
    }
    ui->comboBoxResizeControl->setCurrentIndex(index);

    dataFile.RetrieveParameter("parentSelection", &stringV);
    index = -1;
    for (size_t i = 0; i < parentSelection.size(); i++)
    {
        ui->comboBoxParentSelection->addItem(QString::fromStdString(parentSelection[i]));
        if (parentSelection[i] == stringV) index = int(i);
    }
    ui->comboBoxParentSelection->setCurrentIndex(index);


    bool boolV;
    dataFile.RetrieveParameter("bounceMutation", &boolV);
    ui->checkBoxBounceMutation->setChecked(boolV);
    dataFile.RetrieveParameter("circularMutation", &boolV);
    ui->checkBoxCircularMutation->setChecked(boolV);
    dataFile.RetrieveParameter("multipleGaussian", &boolV);
    ui->checkBoxMultipleGaussian->setChecked(boolV);
    dataFile.RetrieveParameter("onlyKeepBestGenome", &boolV);
    ui->checkBoxOnlyKeepBestGenome->setChecked(boolV);
    dataFile.RetrieveParameter("onlyKeepBestPopulation", &boolV);
    ui->checkBoxOnlyKeepBestPopulation->setChecked(boolV);
    dataFile.RetrieveParameter("randomiseModel", &boolV);
    ui->checkBoxRandomiseModel->setChecked(boolV);
    dataFile.RetrieveParameter("minimizeScore", &boolV);
    ui->checkBoxMinimizeScore->setChecked(boolV);

    double doubleV;
    dataFile.RetrieveParameter("crossoverChance", &doubleV);
    ui->doubleSpinBoxCrossoverChance->setValue(doubleV);
    dataFile.RetrieveParameter("duplicationMutationChance", &doubleV);
    ui->doubleSpinBoxDuplicationMutationChance->setValue(doubleV);
    dataFile.RetrieveParameter("frameShiftMutationChance", &doubleV);
    ui->doubleSpinBoxFrameShiftMutationChance->setValue(doubleV);
    dataFile.RetrieveParameter("gamma", &doubleV);
    ui->doubleSpinBoxGamma->setValue(doubleV);
    dataFile.RetrieveParameter("gaussianMutationChance", &doubleV);
    ui->doubleSpinBoxGaussianMutationChance->setValue(doubleV);
    dataFile.RetrieveParameter("improvementThreshold", &doubleV);
    ui->doubleSpinBoxImprovementThreshold->setValue(doubleV);
    dataFile.RetrieveParameter("watchDogTimerLimit", &doubleV);
    ui->doubleSpinBoxWatchDogTimerLimit->setValue(doubleV);

    int intV;
    dataFile.RetrieveParameter("genomeLength", &intV);
    ui->spinBoxGenomeLength->setValue(intV);
    dataFile.RetrieveParameter("improvementReproductions", &intV);
    ui->spinBoxImprovementReproductions->setValue(intV);
    dataFile.RetrieveParameter("maxReproductions", &intV);
    ui->spinBoxMaxReproductions->setValue(intV);
    dataFile.RetrieveParameter("outputPopulationSize", &intV);
    ui->spinBoxOutputPopulationSize->setValue(intV);
    dataFile.RetrieveParameter("outputStatsEvery", &intV);
    ui->spinBoxOutputStatsEvery->setValue(intV);
    dataFile.RetrieveParameter("parentsToKeep", &intV);
    ui->spinBoxParentsToKeep->setValue(intV);
    dataFile.RetrieveParameter("populationSize", &intV);
    ui->spinBoxPopulationSize->setValue(intV);
    dataFile.RetrieveParameter("saveBestEvery", &intV);
    ui->spinBoxSaveBestEvery->setValue(intV);
    dataFile.RetrieveParameter("savePopEvery", &intV);
    ui->spinBoxSavePopEvery->setValue(intV);

}

void GAParametersDialog::activateButtons()
{
    if (ui->comboBoxCrossoverType->currentIndex() == -1 || ui->comboBoxParentSelection->currentIndex() == -1 || ui->comboBoxResizeControl->currentIndex() == -1)
    {
        ui->pushButtonOK->setEnabled(false);
    }
    else
    {
        ui->pushButtonOK->setEnabled(true);
    }
}
