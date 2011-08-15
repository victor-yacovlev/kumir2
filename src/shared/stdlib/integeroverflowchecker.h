//
// C++ Interface: integeroverflowchecker
//
// Description:
//
//
// Author: Виктор Яковлев <victor@lpm.org.ru>, (C) 2008-2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INTEGEROVERFLOWCHECKER_H
#define INTEGEROVERFLOWCHECKER_H

#include <QtCore>


#ifdef STDLIB_LIBRARY
#define STDLIB_EXPORT Q_DECL_EXPORT
#else
#define STDLIB_EXPORT Q_DECL_IMPORT
#endif

namespace StdLib {

/**
        @author Виктор Яковлев <victor@lpm.org.ru>
*/
class STDLIB_EXPORT IntegerOverflowChecker
{
public:
    /**
     * 32 или 32/64-битная архитектура
     */
    enum Architecture { Always32Bit, SystemDependent };

    /**
     * Установка типа архитектуры
     * @param arch тип архитектуры
     */
    inline static void setArchitecture(Architecture arch) { m_architecture = arch; updateN(); }

    /**
     * Возвращает тип архитектуры
     * @return тип архитектуры
     */
    inline static Architecture architecture() { return m_architecture; }

    /**
     * Проверка операции сложения
     * @param first первое слагаемое
     * @param second второе слагаемое
     * @return возможность выполнения операции
     */
    static bool checkSumm(const int first, const int second);

    /**
      * Проверка операции вычитания
      * @param first уменьшаемое
      * @param second вычитаемое
      * @return возможность выполнения операции
      */
    static bool checkDiff(const int first, const int second);

    /**
      * Проверка операции умножения
      * @param first первый множитель
      * @param second второй множитель
      * @return возможность выполнения операции
      */
    static bool checkProd(const int first, const int second);

    /**
      * Проверка операции возведения в неотрицательную степень
      * @param first основание
      * @param second показатель
      * @return возможность выполнения операции
      */
    static bool checkPower(const int first, const uint second);

    /**
      * Проверка операции преобразования из вещественного числа
      * @param value вещественное значение
      * @return возможность выполнения операции
      */
    static bool checkFromReal(const qreal value);

    /**
      * Проверка операции преобразования из вещественного числа
      * @param value строковое представление числа
      * @return возможность выполнения операции
      */
    static bool checkFromString(const QString &value);

private:
    static void updateN();
    static Architecture m_architecture;
    static uint n;
    static qreal log_2(const qreal x);
    static int ipow(int base, uint pwr);
    static int power2inNminus2;

};

}

#endif
