/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _SVDVITER_HXX
#define _SVDVITER_HXX

#include <tools/solar.h>
#include <sal/types.h>
#include "svx/svxdllapi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

class OutputDevice;
class Window;
class SdrView;
class SdrPageView;
class SdrModel;
class SdrPage;
class SdrObject;
class SetOfByte;

////////////////////////////////////////////////////////////////////////////////////////////////////

class SVX_DLLPUBLIC SdrViewIter 
{
	const SdrModel*										mpModel;
	const SdrPage*										mpPage;
	const SdrObject*									mpObject;
	SdrView*											mpAktView;

	sal_uInt32											mnListenerNum;
	sal_uInt32											mnPageViewNum;
	sal_uInt32											mnOutDevNum;

	// bitfield
	unsigned											mbNoMasterPage : 1;

private:
	SVX_DLLPRIVATE void          ImpInitVars();
	SVX_DLLPRIVATE SdrView*      ImpFindView();
	SVX_DLLPRIVATE SdrPageView*  ImpFindPageView();
	SVX_DLLPRIVATE OutputDevice* ImpFindOutDev();
	SVX_DLLPRIVATE Window*       ImpFindWindow();
	SVX_DLLPRIVATE sal_Bool		 ImpCheckPageView(SdrPageView* pPV) const;

public:
	SdrViewIter(const SdrModel* pModel);
	SdrViewIter(const SdrPage* pPage, sal_Bool bNoMasterPage = sal_False);
	SdrViewIter(const SdrObject* pObject, sal_Bool bNoMasterPage = sal_False);

	SdrView* FirstView();
	SdrView* NextView();

	SdrPageView* FirstPageView();
	SdrPageView* NextPageView();

	OutputDevice* FirstOutDev();
	OutputDevice* NextOutDev();

	Window* FirstWindow();
	Window* NextWindow();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _JUST_DESCRIPTION

Mit dieser Klasse kann man rausbekommen:
- SdrView* First/NextView()
  - Alle Views in denen ein Models dargestellt wird
  - Alle Views in denen eine bestimme Page sichtbar ist (ww. auch als MasterPage)
  - Alle Views in denen ein bestimmes Objekt sichtbar ist (ww. auch auf MasterPage)
- SdrPageView* First/NextPageView()
  - Alle PageViews in denen ein Models dargestellt wird
  - Alle PageViews in denen eine bestimme Page sichtbar ist (ww. auch als MasterPage)
  - Alle PageViews in denen ein bestimmes Objekt sichtbar ist (ww. auch auf MasterPage)
- OutputDevice* First/NextOutDev()
  - Alle OutputDevices in denen ein Models dargestellt wird
  - Alle OutputDevices in denen eine bestimme Page sichtbar ist (ww. auch als MasterPage)
  - Alle OutputDevices in denen ein bestimmes Objekt sichtbar ist (ww. auch auf MasterPage)
- Window* First/NextWindow()
  - Alle Windows in denen ein Models dargestellt wird
  - Alle Windows in denen eine bestimme Page sichtbar ist (auch als MasterPage)
  - Alle Windows in denen ein bestimmes Objekt sichtbar ist (auch auf MasterPage)
Ob die Auswahl auf ein(e) bestimmte(s) Page/Objekt beschraenkt wird, bestimmt man
durch die Wahl des Konstruktors.

Es werden u.a. auch berueksichtigt:
- Layer Sichtbarkeitsstatus
- Visible Layer von MasterPages
- Mehrfachlayer bei Gruppenobjekten

Es wird nicht berueksichtigt:
- Ob die Pages/Objekte wirklich schon gepaintet wurden oder noch ein Invalidate ansteht, ...
- Ob die Pages/Objekte in einem Window im sichtbaren Bereich liegen

#endif // _JUST_DESCRIPTION

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_SVDVITER_HXX

