/*
 Copyright (c) 2013, Insomniac Games
 
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
 - Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 disclaimer.
 - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 \file
 \author Ron Pieket \n<http://www.ItShouldJustWorkTM.com> \n<http://twitter.com/RonPieket>
 */
/* MojoLib is documented at: http://www.itshouldjustworktm.com/mojolib/ */
#pragma once

// -- Common
#include "MojoConstants.h"
#include "MojoStatus.h"
#include "MojoUtil.h"
#include "MojoAlloc.h"
#include "MojoConfig.h"
#include "MojoSet.h"

// -- Containers
#include "MojoKeyValue.h"
#include "MojoCollector.h"
#include "MojoSet.h"
#include "MojoMap.h"
#include "MojoMultiMap.h"
#include "MojoArray.h"
#include "MojoRelation.h"

// -- Id
#include "MojoId.h"
#include "MojoIdManager.h"

// -- Boolean Sets
#include "MojoAbstractSet.h"
#include "MojoCacheSet.h"
#include "MojoIntersection.h"
#include "MojoUnion.h"
#include "MojoDifference.h"
#include "MojoComplement.h"

// -- Transformational Sets
#include "MojoFnDirectOpenShallow.h"
#include "MojoFnDirectOpenDeep.h"
#include "MojoFnDirectClosedShallow.h"
#include "MojoFnDirectClosedDeep.h"
#include "MojoFnInverseOpenShallow.h"
#include "MojoFnInverseOpenDeep.h"
#include "MojoFnInverseClosedShallow.h"
#include "MojoFnInverseClosedDeep.h"
