#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class main_window; }
QT_END_NAMESPACE

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    main_window(QWidget *parent = 0);
    ~main_window();

private slots:
    void on_change_colonie_button_clicked();

    void on_prg_arrow_up_btn_clicked();

    void on_prg_arrow_back_btn_clicked();

    void on_prg_arrow_left_btn_clicked();

    void on_prg_arrow_center_btn_clicked();

    void on_prg_arrow_right_btn_clicked();

    void on_prg_arrow_down_btn_clicked();

    void on_m_c_start_btn_clicked();

    void on_m_c_cancel_btn_clicked();

    void on_m_c_step_btn_clicked();

    void on_m_c_auto_btn_clicked();

    void on_m_c_stop_btn_clicked();

private:
    Ui::main_window *ui;
};
#endif // MAIN_WINDOW_H
