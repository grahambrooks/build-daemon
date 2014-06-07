class is {
public:
  static bool verbose_option(const char *option) {
    return not_null(option) && switch_char(option[0]) && verbose_char(option[1]) && eos(&option[2]);
  }

  static bool trigger_option(const char* option) {
      return not_null(option) && switch_char(option[0]) && trigger_char(option[1]) && eos(&option[2]);
  }

  static bool not_null(const char *t) {
    return t != NULL;
  }

  static bool switch_char(const char c) {
    return c == '-';
  }

  static bool verbose_char(const char c) {
    return c == 'v';
  }

  static bool trigger_char(const char c) {
      return c == 't';
  }

  static bool eos(const char *c) {
    return *c == 0;
  };
};

