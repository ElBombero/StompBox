#ifndef UTILS_H
#define UTILS_H


template<class T>
static T Min(T t1, T t2)
{
  return (t1 < t2) ? t1 : t2;
}

template<class T>
static T Max(T t1, T t2)
{
  return (t1 > t2) ? t1 : t2;
}

#endif // defined UTILS_H