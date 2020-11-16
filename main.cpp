#include <iostream>
#include <unistd.h>
#include <climits>
#include <libgen.h>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <algorithm>

static char* program_name;
static std::string program_path;
static std::string cinder_path;
static std::string template_name;
static std::string app_name;

void usage() {
  std::cout << "Usage: " << program_name << " <options> project_name" << '\n'
    << '\n' << "Options:" << '\n'
    << "  " << "-h this help message"<< '\n'
    << "  " << "-t template name" << '\n'
    << "  " << "-p path to cinder" << '\n'
    << '\n' << "Examples:" << '\n'
    << "  " << program_name << " -t \"basic\" -p ~/Cinder BasicApp" << '\n';
  exit(1);
}

void parse_arguments(int argc, char** argv) {

  program_name = argv[0];

  int option;

  while((option = getopt(argc, argv, ":t:p:h")) != -1) {
    switch(option){
      case 't':
        template_name = optarg;
        break;
      case 'p':
        cinder_path = optarg;
        break;
      case 'h':
      case ':':
      case '?':
      default:
        usage();
        break;
    }
  }

  if (optind < argc) {
    app_name = argv[optind];
    optind++;
  } else {
    usage();
  }

  if (optind < argc) {
    usage();
  }

}

std::string get_program_path() {
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  const char *path = nullptr;
  if (count != -1) {
    path = dirname(result);
  } else {
    std::cout << "bug in get_program_path()" << '\n';
    exit(2);
  }
  return path;
}

std::string project_root() {
  return program_path.substr(0, program_path.find_last_of('/'));
}

std::string templates_dir() {
  return project_root() + "/templates";
}

void load_defaults() {
  const std::string defaults_path = project_root() + "/defaults";

  std::ifstream stream(defaults_path);
  if (stream) {
    std::string line;
    while(std::getline(stream, line)) {
      const std::string key = line.substr(0,line.find_first_of('='));
      const std::string value = line.substr(line.find_first_of('=') + 1, line.size());
      if (key == "CINDER_PATH") {
        if (cinder_path.empty()) {
          cinder_path = value;
        }
      } else if (key == "TEMPLATE") {
        if (template_name.empty()) {
          template_name = value;
        }
      }
    }
  }
}

void verify_arguments() {
  // TODO: Add more checks using regex to project_name and cinder_path

  std::vector<std::string> templates;

  for (const auto &entry: std::filesystem::directory_iterator(templates_dir())) {
    const std::string path = entry.path().string();
    templates.push_back(path.substr(path.find_last_of('/') + 1, path.size()));
  }

  bool is_valid_template = false;
  for (const auto &temp : templates) {
    if (temp == template_name) {
      is_valid_template = true;
      break;
    }
  }

  if (!is_valid_template) {
    std::cout << "Invalid template name \"" << template_name << "\""<< '\n'
      << '\n' << "valid names:" << '\n';

    for (const auto &temp : templates) {
      std:: cout << "  " << temp << '\n';
    }

    exit(3);
  }

  if (cinder_path.empty()) {
    std::cout << "No cinder path specified! -h for more info or see readme file on setting a default." << '\n';
    exit(4);
  }

}

std::string app_dir() {
  return "./" + app_name;
}

void copy_template() {
  std::filesystem::create_directory(app_dir());
  std::filesystem::copy(templates_dir() + "/" + template_name, app_dir(), std::filesystem::copy_options::recursive);
}

void replace_filenames_recursive(const std::string &dir) {
  for (const auto &entry: std::filesystem::directory_iterator(dir)) {

    if (std::filesystem::is_directory(entry)) {
      replace_filenames_recursive(entry.path());
    }

    // TODO: replace with regular expression
    if (entry.path().string().find("%APP_NAME%") != std::string::npos) {
      const std::string old_name = entry.path().string();
      const std::string new_name = old_name.substr(0, old_name.find_first_of('%')) + app_name + old_name.substr(old_name.find_last_of('%') + 1, old_name.size());
      std::filesystem::rename(old_name, new_name);
    }
  }
}

void string_replace(std::string &s, const std::string& to_find, const std::string& new_value) {
  while(s.find(to_find) != std::string::npos) {
    s.replace(s.find(to_find), to_find.size(), new_value);
  }
}

void replace_in_files_recursive(const std::string &dir) {
  for (const auto &entry: std::filesystem::directory_iterator(dir)) {
    if (std::filesystem::is_directory(entry)) {
      replace_in_files_recursive(entry.path());
    } else {

      const std::string temp_file_path = dir + "/temp";

      std::ifstream file_in(entry.path());
      std::ofstream file_out(temp_file_path);

      if (!file_in || !file_out) {
        std::cout << "Unable to open file in replace_in_files_recursive()." << '\n';
        exit(5);
      }

      std::string s;
      while (std::getline(file_in, s)) {

        string_replace(s, "%APP_NAME%", app_name);
        string_replace(s, "%CINDER_PATH%", cinder_path);

        file_out << s << '\n';

      }

      file_in.close();
      file_out.close();

      std::filesystem::remove(entry.path());
      std::filesystem::rename(temp_file_path, entry.path());

    }
  }
}

void replace_variables() {
  replace_filenames_recursive(app_dir());
  replace_in_files_recursive(app_dir());
}

int main(int argc, char** argv) {

  parse_arguments(argc, argv);

  program_path = get_program_path();

  load_defaults();

  verify_arguments();

  copy_template();
  replace_variables();

//  std::cout << template_name << '\n';
//  std::cout << cinder_path << '\n';

//  std::cout << "Hello, World!" << std::endl;
  return 0;
}
