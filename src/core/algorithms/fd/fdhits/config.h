
#pragma once

namespace algos::fdhits {

struct Config {
    // Различные параметры конфигурации для алгоритма FD-HITS
    
    // Размер выборки для hitting set
    unsigned sample_size = 30;
    
    // Максимальное время выполнения в секундах
    double timeout = 3600;
    
    // Использовать многопоточность
    bool parallel = true;
    
    // Количество потоков (0 - автоматический выбор)
    unsigned threads = 0;
};

}  // namespace algos::fdhits
