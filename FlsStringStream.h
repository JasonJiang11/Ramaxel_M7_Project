#pragma once
#include <tester.h>
#include <string>
#include <iomanip>


//Needs to be replaced with header update from system software
enum class FLS_EXT
{
	PV_FAIL = 0x0,
	PF_FAIL = 0x1,
	SF_FAIL = 0x8
};

//Would be nice if this was available from system software
std::string cid_names[16] = {
	"CID_NOP",
	"CID_FAIL",
	"CID_EXT",
	"CID_FFAIL",
	"CID_TEC",
	"CID_ERROR",
	"CID_CMD",
	"CID_RSVD7",
	"CID_FLUSH",
	"CID_USER1",
	"CID_USER2",
	"CID_USER3",
	"CID_USER4",
	"CID_USER5",
	"CID_DEBUG",
	"CID_HALTED"
};

void compstr_append_cmd(std::stringstream& ss, const COMP128& comp, bool use_color)
{
	switch (comp.id.cid)
	{
	case CID_CMD: ss << "CID_CMD   "; break;
	case CID_ERROR: ss << (use_color ? "<COLOR=RED>" : "") << "CID_ERROR " << (use_color ? "</COLOR>" : ""); break;
	case CID_HALTED: ss << (use_color ? "<COLOR=RED>" : "") << "CID_HALTED" << (use_color ? "</COLOR>" : ""); break;
	default: ss << "CID_???   ";
	}
	ss << std::dec << ": cmd=" << comp.cmd.cmd << " read=" << comp.cmd.read <<
		" error=" << comp.cmd.error << " forced=" << comp.cmd.forced;
}

void compstr_append_ext(std::stringstream& ss, const COMP128& comp, bool use_color)
{
#if 0
	//FIXME comp.id.ext_id isn't supported by headers yet
	switch ((FLS_EXT)comp.id.ext_id) {
	case FLS_EXT::PV_FAIL:
		ss << "CID_EXT: FLS_EXT::PV_FAIL currently unsupported by debug output.";
		break;
	case FLS_EXT::PF_FAIL:
		ss << "CID_EXT: FLS_EXT::PF_FAIL currently unsupported by debug output.";
		break;
	case FLS_EXT::SF_FAIL:
		ss << "CID_EXT: FLS_EXT::SF_FAIL" <<
			", dut=" << std::uppercase << std::setfill('0') << std::setw(2) << std::dec << comp.single_fixed.dutnum + 1 <<
			", addr=0x" << std::uppercase << std::setfill('0') << std::setw(10) << std::hex << comp.single_fixed.addr <<
			", pin=" << std::uppercase << std::setfill('0') << std::setw(2) << std::dec << comp.single_fixed.pinnum <<
			", ui=" << std::uppercase << std::setfill('0') << std::setw(1) << std::dec << comp.single_fixed.fseq;
		break;
	default:
		ss << "CID_EXT:: Unrecognized ext_id of " << std::to_string(comp.id.ext_id);
	}
#endif
	return;
}
void compstr_append_user(std::stringstream& ss, const COMP128& comp, bool use_color)
{
	switch (comp.id.cid)
	{
	case CID_FLUSH: ss << (use_color ? "<COLOR=BLUE>" : "") << "CID_FLUSH " << (use_color ? "</COLOR>" : ""); break;
	case CID_USER1:
	case CID_USER2:
	case CID_USER3:
	case CID_USER4:
	case CID_USER5:
		ss << "CID_USER" << int(comp.id.cid - CID_USER1 + 1) << " ";
		break;
	default: ss << "CID_???   ";
	}
	ss << ": forced=" << comp.user.forced <<
		", ureg=0x" << std::uppercase << std::setfill('0') << std::setw(10) << std::hex << comp.user.ureg <<
		", x=0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << comp.user.xaddr <<
		", y=0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << comp.user.yaddr <<
		", z=0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << comp.user.zaddr;
}

void fls_to_ss(std::stringstream& ss, const COMP128& comp, bool use_color)
{
	switch (comp.id.cid) {
	case CID_NOP: ss << (use_color ? "<COLOR=GRAY>" : "") << "CID_NOP   " << (use_color ? "</COLOR>:" : ""); break;
	case CID_FFAIL:
	case CID_FAIL:
		ss << (use_color ? "<COLOR=RED>" : "") << (comp.id.cid == CID_FAIL ? "CID_FAIL  " : "CID_FFAIL ") <<
			(use_color ? "</COLOR>" : "") << ": addr=0x" << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << comp.fail.addr <<
			", fails(" << (comp.fail.hi ? "127:64" : " 63:0 ") <<
			")=0x" << std::uppercase << std::setfill('0') << std::setw(18) << std::hex << comp.fail.fails;
		break;
		//	case CID_EXT:   //FIXME CID_EXT is not in the HD7 header file
	case CID_RSVD2:
		compstr_append_ext(ss, comp, use_color); break;
	case CID_RSVD7:
		ss << "<COLOR=GREY>CID_RSVD  </COLOR>:"; break;
	case CID_TEC:
		ss << "CID_TEC   : [SUB1] flags=" <<
			(comp.tec.sub[1].last ? 'L' : 'l') << (comp.tec.sub[1].allZero ? 'Z' : 'z') << (comp.tec.sub[1].valid ? 'V' : 'v') << (comp.tec.sub[1].oflow ? 'O' : 'o') <<
			" dut=" << std::setfill('0') << std::setw(3) << comp.tec.sub[1].dut <<
			" reg=" << std::setfill('0') << std::setw(3) << comp.tec.sub[1].region <<
			" count=" << std::setfill('0') << std::setw(9) << comp.tec.sub[1].count <<
			" | [SUB0] flags=" <<
			(comp.tec.sub[0].last ? 'L' : 'l') << (comp.tec.sub[0].allZero ? 'Z' : 'z') << (comp.tec.sub[0].valid ? 'V' : 'v') << (comp.tec.sub[0].oflow ? 'O' : 'o') <<
			" dut=" << std::setfill('0') << std::setw(3) << comp.tec.sub[0].dut <<
			" reg=" << std::setfill('0') << std::setw(3) << comp.tec.sub[0].region <<
			" count=" << std::setfill('0') << std::setw(9) << comp.tec.sub[0].count;
		break;
	case CID_ERROR:
	case CID_CMD:
	case CID_HALTED:
		compstr_append_cmd(ss, comp, use_color); break;
	case CID_FLUSH:
	case CID_USER1:
	case CID_USER2:
	case CID_USER3:
	case CID_USER4:
	case CID_USER5:
		compstr_append_user(ss, comp, use_color); break;
	case CID_DEBUG:
		//CStr.AppendFormat("CID_DEBUG : Type=%d TimeStamp=%d", comp.timer.type, comp.timer.timer); break;
		ss << "CID_DEBUG : Type=" << comp.timer.type << " TimeStamp=" << comp.timer.timer;
		ss << " (" << (fls_tickcount_to_time(comp.timer.timer) / (1 US)) << "us)";
		break;
	}
	ss << std::endl;
	return;
}

