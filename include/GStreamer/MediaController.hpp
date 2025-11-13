#pragma once
#include "GStreamer/Manager.hpp"
#include <gtkmm.h>

class MediaController : public Gtk::Window
{
public:
    explicit MediaController(GstManager &externalManager);
    ~MediaController();

private:
    Gtk::VBox vbox;
    GstManager &manager;
    Gtk::Button btn_pause, btn_resume, btn_restart, btn_exit;
    Gtk::Label status_label;

    void on_pause_clicked();
    void on_resume_clicked();
    void on_restart_clicked();
    void on_exit_clicked();
};
