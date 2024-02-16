template <typename T>
void Logger::print(const T& message) {
    Serial.print(message);
}

template <typename T>
void Logger::println(const T& message) {
    Serial.println(message);
}
