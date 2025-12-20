#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include <fstream>
#include <string>
using namespace std;

void createDirectory(string);
string getCurrentWallpaper();
void getWallpapers(string&, Gtk::FlowBox*&);
void reloadWallpapers(string&, Gtk::FlowBox*&);
void getDirectories(string&, Glib::RefPtr<Gtk::StringList>&);

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
    auto dropDown = builder->get_widget<Gtk::DropDown>("zeDropDown");
    Glib::RefPtr<Gio::ListModel> PtrList = dropDown->get_model(); //get the model from the drop down, which is the StringList
    Glib::RefPtr<Gtk::StringList> directoryList = dynamic_pointer_cast<Gtk::StringList>(PtrList); //cast from a pointer (what get_model() poops out) to a regular old StringList object (so append method works)

    container->add_css_class("background");
    setWallpaperBtn->add_css_class("btn");
    settingsBtn->add_css_class("btn");

    string homeDir = getenv("HOME");
    string wallDirectory = homeDir + "/Pictures/Wallpapers"; //change this and recompile if you wanna' change where wallpapers are stored.
    string curWallDirectory = wallDirectory;
    createDirectory(wallDirectory);
    selectedWallpaper->set_filename(getCurrentWallpaper());
    selectedWallpaper->add_css_class("current_wallpaper");
    selectedWallpaper->set_size_request(400,320);
    selectedWallpaper->set_content_fit(Gtk::ContentFit::COVER);
    getWallpapers(curWallDirectory, grid);
    getDirectories(wallDirectory, directoryList);

    grid->signal_child_activated().connect([selectedWallpaper](Gtk::FlowBoxChild* child) {
        auto wallpaperSelection = dynamic_cast<Gtk::Picture*>(child->get_child());
        if (wallpaperSelection) {
            auto selWallFile = wallpaperSelection->get_file();
            string selWallPath = selWallFile->get_path().c_str();
            cout << "wallpaper " << selWallPath << " selected" << endl;
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
    dropDown->property_selected().signal_changed().connect([dropDown, wallDirectory, directoryList, &curWallDirectory, &grid]() {
        auto childIndex = dropDown->get_selected();
        string chDirName = directoryList->get_string(childIndex);
        curWallDirectory = wallDirectory + "/" + chDirName;
        cout << "current wallpaper directory changed! -> " << curWallDirectory << endl;
        // reload grid
        reloadWallpapers(curWallDirectory, grid);
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
    vector<string> wallpapers; //vector to store wallpaper objects
    for (const auto & entry : filesystem::directory_iterator(wD)) {
        if (!entry.is_directory()) {
            string path = entry.path();
            wallpapers.push_back(path);
            auto curWallpaper = Gtk::make_managed<Gtk::Picture>();
            curWallpaper->add_css_class("current_wallpaper");
            curWallpaper->set_size_request(260,150);
            curWallpaper->set_content_fit(Gtk::ContentFit::COVER);
            curWallpaper->set_filename(path);
            grid->append(*curWallpaper);
        }
    }
}
// possibly combine these at a later time to improve disk efficiency (I don't think this contributes much, but an extra read is an extra read).
void getDirectories(string& wD, Glib::RefPtr<Gtk::StringList>& directoryList) {
    vector<string> directories; //store directories in vector
    int workingDirCharCount = wD.length() + 1; //Wallpaper directory char count, plus one to include the extra backslash.
    string directoryNm = "blank";
    for (const auto & entry : filesystem::directory_iterator(wD)) {
        if (entry.is_directory()) {
            string path = entry.path();
            directories.push_back(path);
            directoryNm = path.erase(0, workingDirCharCount);
            directoryList->append(directoryNm);
        }
    }
}

void reloadWallpapers(string& wD, Gtk::FlowBox*& grid) {

    while (auto child = grid->get_first_child()) {
        grid->remove(*child);
    } cout << "cleared grid!" << endl;

    vector<string> wallpapers; //vector to store wallpaper objects
    for (const auto & entry : filesystem::directory_iterator(wD)) {
        if (!entry.is_directory()) {
            string path = entry.path();
            wallpapers.push_back(path);
            auto curWallpaper = Gtk::make_managed<Gtk::Picture>();
            curWallpaper->add_css_class("current_wallpaper");
            curWallpaper->set_size_request(260,150);
            curWallpaper->set_content_fit(Gtk::ContentFit::COVER);
            curWallpaper->set_filename(path);
            grid->append(*curWallpaper);
            //cout << path << endl;
        }
    }
}