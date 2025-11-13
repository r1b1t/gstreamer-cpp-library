#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include "GStreamer/MediaController.hpp"
#include <gtkmm/application.h>
#include <thread>

int main(int argc, char *argv[])
{
    GstManager manager;
    manager.addPipeline(GstPipelineBuilder::windowscam(640, 480));

    // Pipeline'ı ayrı bir thread'de çalıştır
    std::thread gst_thread([&manager]()
                           { manager.run(); });

    // GTK arayüzü ana thread'de çalışır
    auto app = Gtk::Application::create(argc, argv, "com.example.gstreamer");
    MediaController console(manager);
    int result = app->run(console);

    // Uygulama kapanınca pipeline thread’ini sonlandır
    if (gst_thread.joinable())
        gst_thread.join();

    return result;
}
