#ifndef LOG_H
#define LOG_H

#include <string>

using namespace std;

class Log
{
public:
   Log(const Log&) = delete;
   Log& operator = (const Log&) = delete;

   static Log& instance()
   {
      static Log _instance;
      return _instance;
   }
   ~Log() {}
   void println();

   void print(string str);
   void println(string str);
   void print(int i);
   void println(int i);

   void print(ostream str);
   void println(ostream str);

private:
   Log() {}             // verhindert, dass ein Objekt von außerhalb von Log erzeugt wird.
                        // protected, wenn man von der Klasse noch erben möchte
};


#endif // LOG_H
