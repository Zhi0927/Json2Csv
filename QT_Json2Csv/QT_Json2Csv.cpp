#include "QT_Json2Csv.h"
#include "ConvertMethods.h"
#include "Common.hpp" 
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>

QT_Json2Csv::QT_Json2Csv(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    this->setWindowTitle("Json2Csv");

    connect(ui.SelectFileBtn, SIGNAL(clicked()), this, SLOT(SelectFileBtn()));
    connect(ui.SelectOutpathBtn, SIGNAL(clicked()), this, SLOT(SelectOutpathBtn()));
    connect(ui.ConvertBtn, SIGNAL(clicked()), this, SLOT(ConvertBtn()));
}

QT_Json2Csv::~QT_Json2Csv()
{}

void QT_Json2Csv::SelectFileBtn() {

    QFileDialog* dialog = new QFileDialog();
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
    dialog->setWindowTitle("Select files");
    dialog->setNameFilter(tr("Text Files(*.txt);; Json File(*.json)"));
    //dialog->setDirectory("C://");
    dialog->setDirectory("D://Test");
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::ExistingFiles);
    dialog->show();
    QStringList files;

    if (dialog->exec())
    {
        std::vector<std::string>().swap(targetfiles);

        files = dialog->selectedFiles();
        ui.ShowInputFiles->setText(files.join("\n"));

        for (const QString& str : files)
        {
            targetfiles.emplace_back(str.toStdString());
        }
    }
}

void QT_Json2Csv::SelectOutpathBtn() {
    QFileDialog* dialog = new QFileDialog();
    dialog->setOption(QFileDialog::ShowDirsOnly, false);
    dialog->setWindowTitle("Select Directory");
    //dialog->setDirectory("C://");
    dialog->setDirectory("D://Test/Save");
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::DirectoryOnly);
    dialog->show();
    QString folder;

    if (dialog->exec())
    {
        targetfolder.clear();

        folder = dialog->selectedFiles()[0];
        ui.ShowSavePath->setText(folder);
        targetfolder = folder.toStdString();
    }
}

void QT_Json2Csv::ConvertBtn() {
    if (targetfolder.empty() || targetfiles.empty()) {
        QMessageBox::warning(NULL, QObject::tr("Warning"), QObject::tr("files or save folder can't be empty!"));
        return;
    }

    QProgressDialog* progressDlg = new QProgressDialog();
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setMinimumDuration(0);
    progressDlg->setWindowTitle("Please Wait...");
    progressDlg->setLabelText("Processing...");
    progressDlg->setCancelButtonText("Cancel");
    progressDlg->setRange(0, targetfiles.size());
    progressDlg->show();

    int i_step = 100 / targetfiles.size();
    for (size_t i = 0; i < targetfiles.size(); i++) {
        progressDlg->setValue(i);   

        if (progressDlg->wasCanceled())
            return;
        std::string outputname = targetfolder + "\\" + remove_extension(base_name(targetfiles[i])) + ".csv";
        Process(targetfiles[i], outputname);
    }
    progressDlg->setValue(targetfiles.size());
    QMessageBox::information(NULL, QObject::tr("Sucessfully"), QObject::tr("Json convert to Csv Sucessfully!"));
}


void QT_Json2Csv::Process(std::string input, std::string output) {

    auto jsonInput = std::make_shared<Json::Value>();

    //check if input file exists
    bool ok = jsonToCsv(jsonInput, input.c_str(), output.c_str());
    if (!ok) {
        exit(1);
    }

    //flatten the objects
    auto objects = jsonToDicts(jsonInput);

    //merge and sort the keys
    auto csv = dictsToCsv(objects);

    //export file
    std::ofstream ofs;
    ofs.open(output);
    for (const auto& e : *csv) {
        auto len = e.get()->size();
        int counter = 0;
        for (const auto& g : *e) {
            ofs << *g << (counter < len - 1 ? "," : "");
            counter++;
        }
        ofs << "\n";
    }
    ofs.close();
}
