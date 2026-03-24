#include <gsl/gsl_eigen.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>

#include <iostream>

#include "../include/compat_platform.h"
#include "../include/define.h"
#include "../include/evolve.h"
#include "../include/io.h"
#include "../include/monte_carlo.h"
#include "../include/parameter_order.h"
#include "../include/parameters.h"
#include "../include/potential.h"
#include "../include/simulator.h"

#define MCLICS_VERSION "0.1"

namespace fs = std::filesystem;

// Remove arquivos de saída de simulações anteriores para evitar mistura de dados.
void cleanPreviousOutput() {
  std::printf("Limpando arquivos de simulacoes anteriores...\n");

  const char* specificFiles[] = {"ic.csv", "po.dat", nullptr};
  for (int i = 0; specificFiles[i] != nullptr; i++) {
    std::remove(specificFiles[i]);
  }

  try {
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
      if (!entry.is_regular_file()) continue;

      const std::string filename = entry.path().filename().string();
      if (filename.rfind("director_field_", 0) == 0 && entry.path().extension() == ".csv") {
        std::error_code ec;
        fs::remove(entry.path(), ec);
      }
    }
  } catch (const std::exception& e) {
    std::fprintf(stderr, "Aviso: nao foi possivel limpar todos os arquivos antigos: %s\n", e.what());
  }

  std::printf("Limpeza concluida!\n\n");
}

// Gera um resumo estatístico baseado no arquivo po.dat.
void printSimulationSummary(Parameters* params) {
  std::printf("\n=== RELATORIO FINAL DA SIMULACAO ===\n");

  FILE* poFile = std::fopen("po.dat", "r");
  if (poFile) {
    double sumS = 0.0, sumE = 0.0;
    double maxS = -1e9, minS = 1e9;
    double maxE = -1e9, minE = 1e9;
    int count = 0;
    char line[256];

    if (std::fgets(line, sizeof(line), poFile) == nullptr) {
      std::fclose(poFile);
      std::printf("Arquivo po.dat vazio ou erro na leitura\n");
      return;
    }

    double T, S, varS, E, varE;
    while (std::fgets(line, sizeof(line), poFile) != nullptr) {
      if (std::sscanf(line, "%lf %lf %lf %lf %lf", &T, &S, &varS, &E, &varE) == 5) {
        sumS += S;
        sumE += E;

        if (S > maxS) maxS = S;
        if (S < minS) minS = S;
        if (E > maxE) maxE = E;
        if (E < minE) minE = E;

        count++;
      }
    }
    std::fclose(poFile);

    if (count > 0) {
      std::printf("Parametro de Ordem (S):\n");
      std::printf("  Media:  %.4f\n", sumS / count);
      std::printf("  Maximo: %.4f\n", maxS);
      std::printf("  Minimo: %.4f\n", minS);

      std::printf("\nEnergia (E):\n");
      std::printf("  Media:  %.4f\n", sumE / count);
      std::printf("  Maxima: %.4f\n", maxE);
      std::printf("  Minima: %.4f\n", minE);

      std::printf("\nEstatisticas:\n");
      std::printf("  Numero de passos salvos: %d\n", count);
    } else {
      std::printf("Nenhum dado valido encontrado em po.dat\n");
    }
  } else {
    std::printf("Arquivo po.dat nao encontrado\n");
  }

  std::printf("\nConfiguracao do Sistema:\n");
  std::printf("  Dimensoes: %dx%dx%d\n", params->Nx, params->Ny, params->Nz);
  std::printf("  Potencial: %s\n", params->potential);
  std::printf("  Geometria: %s\n", params->geometry);
  std::printf("  Condicoes de contorno: %s / %s / %s\n", params->XBoundtype, params->YBoundtype, params->ZBoundtype);
  std::printf("====================================\n\n");
}

int main(int argc, char** argv) {
  std::printf("### Starting McLiCS version: %s ###\n\n", MCLICS_VERSION);

  if (argc < 2) {
    std::printf("Erro: Arquivo de entrada nao especificado.\n");
    std::printf("Uso: %s <arquivo_de_parametros>\n", argv[0]);
    return 1;
  }

  cleanPreviousOutput();

  Parameters params = read_input_file(argv[1]);
  print_parameters(params);

  simulator* sim = new simulator(&params);
  sim->Setup_simmulation(params);

  sim->print_n("ic.csv", &params);
  sim->evolve->run();

  printSimulationSummary(&params);

  delete sim;
  return 0;
}
