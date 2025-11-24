#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include <fstream>
#include <string>
using namespace std;

void createDirectory(string);
string getCurrentWallpaper();
void getWallpapers(string&, Gtk::FlowBox*&);

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("org.lforsythe.hyprwallpaper", Gio::Application::Flags::NON_UNIQUE);
    auto css = Gtk::CssProvider::create();

    auto builder = Gtk::Builder::create_from_file(ROOT"/hyprwallpaper.ui");
    auto pWindow = builder->get_widget<Gtk::Window>("DaWindow");
    css->load_from_path(ROOT"/style.css");
    Gtk::StyleProvider::add_provider_for_display(pWindow->get_display(), css,
  GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    auto grid = builder->get_widget<Gtk::FlowBox>("DaGrid");
    auto selectedWallpaper = builder->get_widget<Gtk::Picture>("selectedWP");
    auto setWallpaperBtn = builder->get_widget<Gtk::Button>("setWallpaperBtn");
    auto container = builder->get_widget<Gtk::Box>("container");
    auto settingsBtn = builder->get_widget<Gtk::Button>("settingsBtn");
    auto settingsDialog = builder->get_widget<Gtk::Dialog>("settingsDialog");
    container->add_css_class("background");
    setWallpaperBtn->add_css_class("btn");

    string homeDir = getenv("HOME");
    string wallDirectory = homeDir + "/Pictures/Wallpapers"; //change this and recompile if you wanna' change where wallpapers are stored.
    createDirectory(wallDirectory);
    selectedWallpaper->set_filename(getCurrentWallpaper());
    selectedWallpaper->add_css_class("current_wallpaper");
    selectedWallpaper->set_size_request(400,320);
    selectedWallpaper->set_content_fit(Gtk::ContentFit::COVER);
    getWallpapers(wallDirectory, grid);

    grid->signal_child_activated().connect([selectedWallpaper](Gtk::FlowBoxChild* child) {
        auto wallpaperSelection = dynamic_cast<Gtk::Picture*>(child->get_child());
        if (wallpaperSelection) {
            cout << "wallpaper selected" << endl;
            auto selWallFile = wallpaperSelection->get_file();
            string selWallPath = selWallFile->get_path().c_str();
            selectedWallpaper->set_filename(selWallPath);
        }
    });
    setWallpaperBtn->signal_clicked().connect([selectedWallpaper]() {
        auto selWallFile = selectedWallpaper->get_file();
        string selWallPath = selWallFile->get_path().c_str();
        system(("hyprctl hyprpaper reload ,\"" + selWallPath + "\"").c_str());
        cout << "wp set to " << selWallPath << endl;
    });
    settingsBtn->signal_clicked().connect([settingsDialog]() {
        settingsDialog->show();
    });

    app->signal_startup().connect([&]() {
        app->add_window(*pWindow);
        app->hold();
        pWindow->set_visible(true);
    });
    return app->run(argc, argv);
}

void createDirectory(string wD) {
    if (!filesystem::exists(wD)) {
        filesystem::create_directory(wD);
    }
    for (const auto & entry : filesystem::directory_iterator(wD))
        cout << entry.path() << endl;

    // create a for iteration that puts each file entry from filesystem::directory_iterator
    // into a string array. Then, create another loop that iterates through the array
    // and creates a new image object for each element in the array (element for each photo in directory)
    // I dunno' how I'm gonna make it so clicking on a photo selects it.
}

string getCurrentWallpaper() {
    ifstream readIn;
    // credit to paxdiablo on stackexchange for solution
    system("hyprctl hyprpaper listloaded > /tmp/current_wallpaper.out"); //read out current wallpaper using hyprpaper, and store this temporarily for read-out.
    readIn.open("/tmp/current_wallpaper.out");
    string currentWallpaper;
    readIn >> currentWallpaper;
    readIn.close();
    cout << currentWallpaper << endl;
    return currentWallpaper;
}

void getWallpapers(string& wD, Gtk::FlowBox*& grid) {
    string wallpapers[100]; //store max 100 wallpapers, which I think should be sufficient, but maybe I'll change to a dynamic array later
    int i = 0;
    for (const auto & entry : filesystem::directory_iterator(wD)) {
        wallpapers[i] = entry.path();
        auto curWallpaper = Gtk::make_managed<Gtk::Picture>();
        curWallpaper->add_css_class("current_wallpaper");
        curWallpaper->set_size_request(280,170);
        curWallpaper->set_content_fit(Gtk::ContentFit::COVER);
        curWallpaper->set_filename(wallpapers[i]);
        grid->append(*curWallpaper);
        i++;
    } //would you just feed file paths directly into new Gtk::Photo objects? Or, would you store them
      //in this array and access them later? I dunno'.
}