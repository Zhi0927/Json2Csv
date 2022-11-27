#ifndef JASON2CSV_H_
#define JASON2CSV_H_

#include <QtWidgets/QWidget>
#include "ui_QT_Json2Csv.h"

class QT_Json2Csv : public QWidget
{
    Q_OBJECT

public:
    QT_Json2Csv(QWidget *parent = nullptr);
    ~QT_Json2Csv();

private slots:
    void SelectFileBtn();
    void SelectOutpathBtn();
    void ConvertBtn();

private:
    void Process(std::string input, std::string output);

private:
    Ui::QT_Json2CsvClass ui;
    std::vector<std::string> targetfiles;
    std::string targetfolder;
};

#endif
