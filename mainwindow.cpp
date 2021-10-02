#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "code.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(823, 619);
    QString sample_path = ":/file/example/file/sample";
    QFile file(sample_path);
    //--打开文件成功
    if (file.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&file);
        while (!textStream.atEnd())
        {
            ui->text_codes->append(textStream.readLine());
        }
    }
    else	//---打开文件失败
        QMessageBox ::information(NULL, NULL, "样例文件丢失");

    // enum phrase_type: int {NONE = 0, KEYWORD = 1, OPERATOR = 2, OTHER = 3, IDENTIFIER = 4, NUMBER=5};
    label_list = new QVector<QTextEdit *>{nullptr, ui->label_keywords, ui->label_operators, ui->label_others, ui->label_identifiers, ui->label_numbers};

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_open_file_triggered()
{

    //---获取文件名
    QString fileName = QFileDialog :: getOpenFileName(this, NULL, NULL, "*.h *.cpp *.txt");

    //---打开文件并读取文件内容
    QFile file(fileName);

    //--打开文件成功
    if (file.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        ui->text_codes->clear();
        QTextStream textStream(&file);
        while (!textStream.atEnd())
        {
            //---QtextEdit按行显示文件内容
            ui->text_codes->append(textStream.readLine());
        }
    }
    else	//---打开文件失败
    {
        /*
            information函数参数说明：
            函数原型
            QMessageBox::information(
            QWidget * parent,
            const QString & title,					//--- 标题
            const QString & text,					//---显示内容
            StandardButtons buttons = Ok,	        //---OK按钮
            StandardButton defaultButton = NoButton)
            [static]
        */
        QMessageBox ::information(NULL, NULL, "打开文件有误");
    }
}

void MainWindow::on_button_update_clicked()
{

    // 本身就是raw
    std::vector<std::unordered_map<std::string, int>> result = code::identify(ui->text_codes->toPlainText().toStdString());
    for(int i = 0; i < label_list->size(); ++i){
         QString qstr;
        if(this->label_list->operator[](i) == nullptr)
            continue;
        for(auto&item : result[i]){
            qstr += QString::fromStdString(item.first) + "   " + QString::number(item.second) + "\n";
        }
        this->label_list->operator[](i)->setText(qstr);
    }

}
