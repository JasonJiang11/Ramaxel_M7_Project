#include "stdafx.h"
#include "public.h"

static void testDataStore()
{
	static apps::DataStore ret;
	ret.reset< WaitForEventStatus >();
	WaitForEventStatus* obj;
	ret.get( t_dut1, 0, obj );
}
static void testMemoryManager()
{
	apps::MemoryManager mm( 128, 32 * 1024 );
	size_t addr;
	size_t numBytes = 120;
	mm.size();
	mm.base();
	mm.query( numBytes );
	mm.allocate( numBytes, addr );
	mm.deallocate( numBytes, addr );
	mm.purge();
}