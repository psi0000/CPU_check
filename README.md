# CPU_check

uint64_t seconds = current_time.seconds();
    uint64_t nanoseconds = current_time.nanoseconds();

    // 초와 나노초를 시간과 분으로 변환
    time_t seconds_since_epoch = static_cast<time_t>(seconds);
    tm local_time;
    localtime_r(&seconds_since_epoch, &local_time);

    std::stringstream time_stream;
    time_stream << local_time.tm_hour << ":" << local_time.tm_min;
