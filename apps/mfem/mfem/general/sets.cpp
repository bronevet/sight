// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.googlecode.com.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.

#include "array.hpp"
#include "table.hpp"
#include "sets.hpp"


IntegerSet::IntegerSet(IntegerSet &s)
   : me(s.me.Size())
{
   for (int i = 0; i < me.Size(); i++)
      me[i] = s.me[i];
}


int IntegerSet::operator== (IntegerSet &s)
{
   if (me.Size() != s.me.Size())
      return 0;

   for (int i = 0; i < me.Size(); i++)
      if (me[i] != s.me[i])
         return 0;

   return 1;
}

int IntegerSet::PickRandomElement()
{
   int i, size = me.Size();
   unsigned int seed = 0;

   for (i = 0; i < size; i++)
      seed += me[i];

   srand(seed);

   return me[rand()/(RAND_MAX/size)];
}

void IntegerSet::Recreate(const int n, const int *p)
{
   int i, j;

   me.SetSize(n);

   for (i = 0; i < n; i++)
      me[i] = p[i];

   me.Sort();

   for (j = 0, i = 1; i < n; i++)
      if (me[i] != me[j])
         me[++j] = me[i];

   me.SetSize(j+1);
}


int ListOfIntegerSets::Insert(IntegerSet &s)
{
   for (int i = 0; i < TheList.Size(); i++)
      if (*TheList[i] == s)
         return i;

   TheList.Append(new IntegerSet(s));

   return TheList.Size()-1;
}

int ListOfIntegerSets::Lookup(IntegerSet &s)
{
   for (int i = 0; i < TheList.Size(); i++)
      if (*TheList[i] == s)
         return i;

   mfem_error("ListOfIntegerSets::Lookup ()");
   return -1;
}

void ListOfIntegerSets::AsTable(Table & t)
{
   int i;

   t.MakeI(Size());

   for (i = 0; i < Size(); i++)
      t.AddColumnsInRow(i, TheList[i] -> Size());

   t.MakeJ();

   for (i = 0; i < Size(); i++)
   {
      Array<int> &row = *TheList[i];
      t.AddConnections(i, row.GetData(), row.Size());
   }

   t.ShiftUpI();
}

ListOfIntegerSets::~ListOfIntegerSets()
{
   for (int i = 0; i < TheList.Size(); i++)
      delete TheList[i];
}
