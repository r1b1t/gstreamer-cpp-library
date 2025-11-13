#include "GStreamer/MediaController.hpp"
#include <cstdlib>
#include <iostream>

MediaController::MediaController(GstManager &externalManager)
    : manager(externalManager),
      btn_pause("Duraklat"), btn_resume("Devam Et"),
      btn_restart("Yeniden Başlat"), btn_exit("Kapat")
{
    set_title("GStreamer Kontrol Paneli");
    set_default_size(300, 200);

    vbox.set_spacing(10);
    vbox.set_border_width(20);
    add(vbox);

    vbox.pack_start(btn_pause, Gtk::PACK_SHRINK);
    vbox.pack_start(btn_resume, Gtk::PACK_SHRINK);
    vbox.pack_start(btn_restart, Gtk::PACK_SHRINK);
    vbox.pack_start(btn_exit, Gtk::PACK_SHRINK);
    vbox.pack_start(status_label, Gtk::PACK_SHRINK);

    btn_pause.signal_clicked().connect(sigc::mem_fun(*this, &MediaController::on_pause_clicked));
    btn_resume.signal_clicked().connect(sigc::mem_fun(*this, &MediaController::on_resume_clicked));
    btn_restart.signal_clicked().connect(sigc::mem_fun(*this, &MediaController::on_restart_clicked));
    btn_exit.signal_clicked().connect(sigc::mem_fun(*this, &MediaController::on_exit_clicked));

    show_all_children();

    status_label.set_text("Durum: Çalışıyor"); // pipeline zaten çalışıyor
}

MediaController::~MediaController() {}

void MediaController::on_pause_clicked()
{
    manager.pause();
    status_label.set_text("Durum: Duraklatıldı");
}

void MediaController::on_resume_clicked()
{
    manager.resume();
    status_label.set_text("Durum: Devam Ediyor");
}

void MediaController::on_restart_clicked()
{
    manager.restart();
    status_label.set_text("Durum: Yeniden Başlatıldı");
}

void MediaController::on_exit_clicked()
{
    std::cout << "Uygulama kapatılıyor...\n";
    manager.~GstManager();
    std::exit(0);
}
