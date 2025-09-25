#include <iostream>
#include <fstream>
#include <string>
#include <sys/utsname.h>
#include <unistd.h>
#include <iomanip>
#include <cstdio>
#include <array>
#include <sstream>
#include <vector>

std::string executar_comando(const std::string& comando) {
    // roda o comando e captura a saída
    std::array<char, 128> buffer;
    std::string resultado;
    FILE* pipe = popen(comando.c_str(), "r");
    if (!pipe) return "Erro ao executar o comando!!";
    
    // Lê a saída do comando linha por linha
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        resultado += buffer.data();
    }
    pclose(pipe);

    // Remove o \n do final se existir
    if (!resultado.empty() && resultado.back() == '\n') {
        resultado.pop_back();
    }
    return resultado;
}

std::string info_cpu(const std::string& key) {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string linha;
    
    while (std::getline(cpuinfo, linha)) {
        if (linha.find(key) != std::string::npos) {
            size_t pos = linha.find(":");
            if (pos != std::string::npos) {
                return linha.substr(pos + 2);
            }
        }
    }
    return "N/A";
}

std::string info_memoria() {
    std::ifstream meminfo("/proc/meminfo");
    std::string linha;
    std::getline(meminfo, linha);
    
    double memoria_kb = std::stod(linha.substr(linha.find(":") + 2));
    double memoria_gb = memoria_kb / (1024 * 1024);
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << memoria_gb << " GB";
    return oss.str();
}

std::string info_uptime() {
    std::ifstream uptime("/proc/uptime");
    double up_seconds;
    uptime >> up_seconds;

    std::ostringstream oss;
    oss << "\t";

    if (up_seconds < 3600) {
        double minutos = up_seconds / 60.0;
        oss << std::fixed << std::setprecision(1) << minutos << " minutos";
    } else {
        double horas = up_seconds / 3600.0;
        oss << std::fixed << std::setprecision(1) << horas << " horas";
    }

    return oss.str();
}


int main() {
    std::vector<std::string> lobo = {
        "                                         ",
        "                            .d$$b        ",
        "                          .' TO$;\\      ",
        "                         /  : TP._;      ",
        "                        / _.;  :Tb|      ",
        "                       /   /   ;j$j      ",
        "                   _.-\"       d$$$$      ",
        "                 .' ..       d$$$$;      ",
        "                /  /P'      d$$$$P. |\\  ",
        "               /   \"      .d$$$P' |\\^\"l ",
        "             .'           `T$P^\"\"\"\"\"  : ",
        "         ._.'      _.'                ; ",
        "      `-.-\".-'-' ._.       _.-\"    .-\" ",
        "    `.-\" _____  ._              .-\"    ",
        "   -(.g$$$$$$$b.              .'       ",
        "     \"\"^^T$$$P^)            .(:        ",
        "       _/  -\"  /.'         /:/;        ",
        "    ._.'-'`-'  \")/         /;/;        ",
        " `-.-\"..--\"\"   \" /         /  ;        ",
        ".-\" ..--\"\"        -'          :        ",
        "..--\"\"--.-\"         (\\      .-(\\      ",
        "  ..--\"\"              `-\\(\\/;`        ",
        "    _.                      :         ",
        "                            ;`-       ",
        "                           :\\         ",
        "                           ;)         "
    };

    std::vector<std::string> infos;

    struct utsname sysinfo;
    if (uname(&sysinfo) == 0) {
        infos.push_back("\t");
        infos.push_back("\t\033[1;31m=== Sistema ===\033[0m");
        infos.push_back(std::string("\tNome: ") + sysinfo.sysname);
        infos.push_back(std::string("\tHostname: ") + sysinfo.nodename);
        infos.push_back(std::string("\tKernel: ") + sysinfo.release);
        infos.push_back(std::string("\tVersão: ") + sysinfo.version);
        infos.push_back(std::string("\tArquitetura: ") + sysinfo.machine);
        infos.push_back("");
    }

    infos.push_back("\t\033[1;31m=== Distribuição ===\033[0m");
    infos.push_back("\tNome: " + executar_comando("lsb_release -d | cut -f2"));
    infos.push_back("\tCodinome: " + executar_comando("lsb_release -c | cut -f2"));
    infos.push_back("");

    infos.push_back("\t\033[1;31m=== CPU ===\033[0m");
    infos.push_back("\tModelo: " + info_cpu("model name"));
    infos.push_back("\tCores: " + info_cpu("cpu cores"));
    infos.push_back("\tThreads: " + info_cpu("siblings"));
    infos.push_back("");

    infos.push_back("\t\033[1;31m=== Memória ===\033[0m");
    infos.push_back("\tTotal: " + info_memoria());
    infos.push_back("");

    infos.push_back("\t\033[1;31m=== Uptime ===\033[0m");
    infos.push_back(info_uptime());
    infos.push_back("");

    infos.push_back("\t\033[1;31m=== Sessão ===\033[0m");
    infos.push_back("\tResolução: " + executar_comando("xrandr | grep '*' | awk '{print $1}' | paste -s -d ' '"));
    infos.push_back("\tDesktop: " + executar_comando("echo $DESKTOP_SESSION"));
    infos.push_back("\t\tWindow Manager: " + executar_comando("echo $XDG_SESSION_DESKTOP"));
    infos.push_back("\t\tTerminal: " + executar_comando("echo $TERM"));
    infos.push_back("\t\tIP Privado: " + executar_comando("ip -4 addr show $(ip route | grep default | awk '{print $5}') | grep inet | awk '{print $2}' | cut -d/ -f1"));

    // lobo + info, lado a lado
    size_t maxLines = std::max(lobo.size(), infos.size());

    for (size_t i = 0; i < maxLines; i++) {
        if (i < lobo.size()) {
            std::cout << lobo[i];
        } else {
            std::cout << "                             ";
        }

        std::cout << "   ";

        if (i < infos.size()) {
            std::cout << infos[i];
        }

        std::cout << "\n";
    }

    return 0;
}
