#include <Preferences.h>

class Storage
{
private:
  Preferences preferences;

public:
  void StoreData(const char *key, const char *val)
  {
    this->preferences.begin("smartlock", false);
    this->preferences.putString(key, val);
    this->preferences.end();
  }

  String ReadData(const char *val, const char *default_val)
  {
    this->preferences.begin("smartlock", false);
    String ret = this->preferences.getString(val, default_val);
    this->preferences.end();
    return ret;
  }
};