#pragma once

#include "Utils.h" // Max / Min

// Обычный (не ref) шаблонный класс — накапливает значения через Add() и отдает
// сумму/минимум/максимум/количество. Тело методов — прямо здесь, в заголовке,
// по той же причине, что и для Max/Min в Utils.h: шаблоны должны быть видны
// целиком в каждом файле, где используются.
template<typename T>
class Stats
{
private:
	T sum;
	T minValue;
	T maxValue;
	int count;
	bool isEmpty;

public:
	Stats()
	{
		count = 0;
		isEmpty = true;
	}

	void Add(T value)
	{
		if (isEmpty)
		{
			sum = value;
			minValue = value;
			maxValue = value;
			isEmpty = false;
		}
		else
		{
			sum = sum + value;
			minValue = Min(minValue, value);
			maxValue = Max(maxValue, value);
		}
		count++;
	}

	T GetSum() { return sum; }
	T GetMin() { return minValue; }
	T GetMax() { return maxValue; }
	int GetCount() { return count; }
};
