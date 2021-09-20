#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <unordered_map>
#include <vector>
#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QTextEdit>
#include <array>
#include <fstream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_open_file_triggered();
    void on_button_update_clicked();

private:
    Ui::MainWindow *ui;
    // enum phrase_type: int {NONE = 0, KEYWORD = 1, OPERATOR = 2, OTHER = 3, IDENTIFIER = 4, NUMBER=5};
    QVector<QTextEdit *> *label_list;
};
#endif // MAINWINDOW_H
