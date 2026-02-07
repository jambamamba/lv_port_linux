#include <algorithm>
#include <filesystem>
#include <optional>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

extern char **environ; // Access to environment variables

//g++ src/font/font.cpp && ./a.out /usr/local/src/libgd-gd-2.3.3/tests/freetype/DejaVuSans.ttf
//g++ -g -O0 font.cpp &&  ./a.out -i /usr/local/src/libgd-gd-2.3.3/tests/freetype/DejaVuSans.ttf -s 8 -s 10 -s 12 -s 14 -s 16 -s 18 -b 1 -b 2 -b 3 -b 4 -b 8 -o /tmp

// Ubuntu-BI.ttf

namespace {

const char *default_font = "/usr/local/src/libgd-gd-2.3.3/tests/freetype/DejaVuSans.ttf";

std::vector<std::string> split_string(const std::string& str_, char delimiter) {
    std::string str(str_); // Create a local copy of the string
    auto new_end = std::remove(str.begin(), str.end(), '\'');
    str.erase(new_end, str.end());
    new_end = std::remove(str.begin(), str.end(), '\n');
    str.erase(new_end, str.end());

    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start)); // Add the last token
    return tokens;
}

std::vector<std::string> getGlyphRanges(const std::string& output) {
    std::vector<std::string> ranges;
    std::vector<std::string> lines = split_string(output, ' ');
    for(auto& line : lines) {
        auto nums = split_string(line, '-');

        int idx = 0;
        std::string range;
        for(auto& num : nums) {
            if(num.size()==0){
                continue;
            }
            if(idx > 0) {
                // printf("-");
                range += '-';
            }
            // printf("0x%s", num.c_str());
            range += "0x" + num;
            ++idx;
        }
        if(range.size()) {
            ranges.push_back(range);
            // printf("range: %s\n", range.c_str());
        }
    }
    return ranges;
}
std::string dump_child_stdout(int fd) {
    char buffer[1024];
    ssize_t bytesRead;
    std::string output;
    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        // printf("%s", buffer); // Print the output from the child process
        for(int i = 0; i < bytesRead; ++i) {
            if(buffer[i] == '\n') {
                output.push_back(' ');
                continue;
            }
            if(buffer[i] == '\'') {
                continue;
            }
            output.push_back(buffer[i]);
        }
        // output += std::string(buffer);
    }
    if (bytesRead == -1) {
        perror("read");
    }
    close(fd); // Close the read end of the pipe after reading
    // printf("### output: %s\n", output.c_str());
    return output;
}

std::vector<char*> makeFcQueryArgs(const std::string &font_file) {
    static std::vector<std::string> args = {};
    args = {
        "fc-query",
        "--format='%{charset}\n'",
        font_file
    };
    std::vector<char*> argv_ptrs;
    for (auto& s : args) {
        argv_ptrs.push_back(&s[0]);
    }
    argv_ptrs.push_back(nullptr);
    return argv_ptrs;
}
std::string fcquery(const std::string &font_file = default_font) {
    pid_t pid;
    std::string cmd("/usr/bin/fc-query");
    auto args = makeFcQueryArgs(font_file);

    posix_spawn_file_actions_t action;
    // 1. Initialize file actions object
    posix_spawn_file_actions_init(&action);
    int out_pipe_fds[2]; // File descriptors for the pipe: [0] for reading, [1] for writing
    // 2. Create the pipe in the parent process
    if (pipe(out_pipe_fds) == -1) {
        perror("pipe");
        return {};
    }
    // 3. Add actions to redirect child's stdout (fd 1) to the write end of the pipe
    // The child's STDOUT_FILENO (1) will be duplicated to the write end (out_pipe_fds[1])
    posix_spawn_file_actions_adddup2(&action, out_pipe_fds[1], STDOUT_FILENO);
    // 4. Add action to close the read end of the pipe in the child process
    // The child doesn't need the read end
    posix_spawn_file_actions_addclose(&action, out_pipe_fds[0]);
    
    // posix_spawn(pid, path, file_actions, attrp, argv_, envp)
    int status = posix_spawn(&pid, cmd.c_str(), &action, nullptr, args.data(), environ);
    if (status != 0) {
        perror("posix_spawn");
        return {};
    }
    // Parent process continues here
    // Close the write end of the pipe in the parent (parent doesn't write, only reads)
    close(out_pipe_fds[1]); 
    
    // printf("Child spawned with PID: %d\n", pid);
    // Wait for the child process to finish
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return {};
    }
    if (!WIFEXITED(status)) {
        printf("Child died an unnatural death.\n");
        return {};
    }
    // printf("Child exit status: %d\n", WEXITSTATUS(status));
    return dump_child_stdout(out_pipe_fds[0]);

    // output should be a string like this:
    // 20-7e a0-2e9 2ec-2ee 2f3 2f7 300-34f 351-353 357-358 35c-362 374-375 37a-37e 384-38a 38c 38e-3a1 3a3-3ce 3d0-486 488-513 531-556 559-55f 561-587 589-58a 5b0-5c3 5c6-5c7 5d0-5ea 5f0-5f4 60c 615 61b 61f 621-63a 640-655 65a 660-66f 674 679-687 691-692 695 698 6a1 6a4 6a6 6a9 6af 6b5 6ba 6bf 6c6 6cc 6ce 6d5 6f0-6f9 7c0-7e7 7eb-7f5 7f8-7fa e3f e81-e82 e84 e87-e88 e8a e8d e94-e97 e99-e9f ea1-ea3 ea5 ea7 eaa-eab ead-eb9 ebb-ebd ec0-ec4 ec6 ec8-ecd ed0-ed9 edc-edd 10a0-10c5 10d0-10fc 1401-1407 1409-141b 141d-1435 1437-144a 144c-1452 1454-14bd 14c0-14ea 14ec-1507 1510-153e 1540-1550 1552-156a 1574-1585 158a-1596 15a0-15af 15de 15e1 1646-1647 166e-1676 1d00-1d14 1d16-1d23 1d26-1d2e 1d30-1d5b 1d5d-1d6a 1d77-1d78 1d7b 1d85 1d9b-1dbf 1dc4-1dc9 1e00-1e9b 1ea0-1ef9 1f00-1f15 1f18-1f1d 1f20-1f45 1f48-1f4d 1f50-1f57 1f59 1f5b 1f5d 1f5f-1f7d 1f80-1fb4 1fb6-1fc4 1fc6-1fd3 1fd6-1fdb 1fdd-1fef 1ff2-1ff4 1ff6-1ffe 2000-2027 202a-2063 206a-2071 2074-208e 2090-2094 20a0-20b5 20d0-20d1 20d6-20d7 2100-2109 210b-2149 214b 214e 2153-2184 2190-22af 22b2-22c6 22c8-22cd 22d6-2311 2318-2319 231c-2321 2324-2328 232b-232c 2373-2375 237a 237d 2387 2394 239b-23ae 23ce-23cf 23e3 23e5 2422-2423 2460-2469 2580-269c 26a0-26b2 2701-2704 2706-2709 270c-2727 2729-274b 274d 274f-2752 2756 2758-275e 2761-2794 2798-27af 27b1-27be 27e0 27e6-27eb 27f0-28ff 2906-2907 290a-290b 2940-2941 2983-2984 29ce-29d5 29eb 29fa-29fb 2a00-2a02 2a0c-2a1c 2a2f 2a7d-2aa0 2aae-2aba 2af9-2afa 2b00-2b1a 2b20-2b23 2c60-2c6c 2c74-2c77 2d30-2d65 2d6f f000-f001 f208 f25f f6c5 fb00-fb06 fb13-fb17 fb1d-fb36 fb38-fb3c fb3e fb40-fb41 fb43-fb44 fb46-fb4f fb52-fb81 fb8a-fb95 fb9e-fb9f fbd9-fbda fbe8-fbe9 fbfc-fbff fe00-fe0f fe70-fe74 fe76-fefc feff fffd 1d300-1d356 1d538-1d539 1d53b-1d53e 1d540-1d544 1d546 1d54a-1d550 1d552-1d56b
}

std::vector<char*> makeLvFontConvArgs(
    const std::string &font_file, 
    const std::string &out_dir, 
    const std::vector<std::string> &ranges,
    const std::vector<std::string> &font_size, 
    const std::vector<std::string> &bpp
) {
    std::string cmd("/usr/local/bin/lv_font_conv");
    static std::vector<std::string> args = {};
    args = {
        "lv_font_conv",
        "--font", font_file,
        "--format", "bin",
        "--no-compress"
    };
    for(const auto& bpp_ : bpp) {
        args.push_back("--bpp");
        args.push_back(bpp_);
    }
    for(const auto& fs : font_size) {
        args.push_back("--size");
        args.push_back(fs);
    }
    for(const auto& range : ranges) {
        args.push_back("--range");
        args.push_back(range);
    }
    args.push_back("-o");
    std::filesystem::path out_path(out_dir);
    if(!std::filesystem::exists(out_path)) {
        std::filesystem::create_directories(out_path);
    }
    std::string output_file = out_dir + "/" + std::filesystem::path(font_file).filename().string() + ".lvf";
    args.push_back(output_file);

    std::vector<char*> argv_ptrs;
    for (auto& s : args) {
        argv_ptrs.push_back(&s[0]);
    }
    argv_ptrs.push_back(nullptr);
    return argv_ptrs;
}
std::optional<std::string> createLvFont(
    const std::vector<std::string>& ranges, 
    const std::vector<std::string>&font_size, 
    const std::vector<std::string>&bpp,
    const std::string &out_dir = "/tmp", 
    const std::string &font_file = default_font) {

// /usr/local/bin/lv_font_conv --font /usr/local/src/libgd-gd-2.3.3/tests/freetype/DejaVuSans.ttf --size 16 --range 0x20-0xFFFF --format bin --bpp 3 --no-compress -o /tmp/output.lvf
// --bpp {1,2,3,4,8}

    // printf("[%s:%i] createLvFont\n", __FILE__, __LINE__);

    std::string cmd("/usr/local/bin/lv_font_conv");
    auto args = makeLvFontConvArgs(font_file, out_dir, ranges, font_size, bpp);
    // for(const auto &arg : args) {
    //     printf("[%s:%i] %s\n", __FILE__, __LINE__, arg);
    // }
    std::string output_file = args[args.size() - 2]; // The output file is the second last argument

    posix_spawn_file_actions_t action;
    posix_spawn_file_actions_init(&action);
    
    pid_t pid;
    int status = posix_spawn(&pid, cmd.c_str(), &action, nullptr, args.data(), environ);
    if (status != 0) {
        perror("posix_spawn");
        return std::nullopt;
    }
    // printf("Child spawned with PID: %d\n", pid);
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return std::nullopt;
    }
    if (!WIFEXITED(status)) {
        // printf("Child died an unnatural death.\n");
        return std::nullopt ;
    }
    // printf("Child exit status: %d\n", WEXITSTATUS(status));
    if(WEXITSTATUS(status)) {
        return std::nullopt;
    }
    return output_file;
}
auto mainArgs(int argc, char *argv[]) {
    std::string output_dir = std::filesystem::current_path();
    std::vector<std::string> font_file;// = {default_font};
    std::vector<std::string> bpp = {"1","2","3","4","8"};
    std::vector<std::string> font_size = {"8","10","12","14","16","18","20","22","24","26","28","30"};
    int opt;
    while ((opt = getopt(argc, argv, "i:s:b:o:h")) != -1) {
        switch(opt) {
            case 'i'://input font file
                font_file.push_back(optarg);
                break;
            case 's'://font size
                font_size.push_back(optarg);
                break;
            case 'b'://bpp
                bpp.push_back(optarg);
                break;
            case 'o'://output directory
                output_dir = optarg;
                break;
            case 'h':
            default:
                fprintf(stderr, "Usage: %s -i <font-file> -s <font-size> -b <bpp> -o <output-dir>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (font_file.size() == 0) {
        fprintf(stderr, "Usage: %s -i <font-file> -s <font-size> -b <bpp> -o <output-dir>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::string>, std::string>{font_file, font_size, bpp, output_dir};
}
}//namespace

int main(int argc, char *argv[]) {
    auto [font_files, font_size, bpp, output_dir] = mainArgs(argc, argv);
    for(const auto &font_file: font_files) {
        auto ranges = getGlyphRanges(fcquery(font_file));
        auto output_file = createLvFont(
            ranges, font_size, bpp, output_dir, font_file
        );
        if(output_file.has_value()) {
            printf("Generated font file: %s\n", output_file.value().c_str());
        } else {
            fprintf(stderr, "Failed to generate font file\n");
        }
    }
    return 0;
}
