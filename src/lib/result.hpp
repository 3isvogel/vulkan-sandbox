template <typename T, typename E> class Result {
public:
  static Result ok(T value) {
    Result tmp;
    tmp.val_ok = value;
    tmp.is_ok = true;
    return tmp;
  }

  static Result err(E value) {
    Result tmp;
    tmp.val_err = value;
    tmp.is_ok = false;
    return tmp;
  }

  inline bool operator~() { return this->is_ok; }

  T ok() { return this->val_ok; }
  E err() { return this->val_err; }

private:
  T val_ok;
  E val_err;
  bool is_ok;
};