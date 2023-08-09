#include <iostream>
#include <iomanip>
#include "stdafx.h"
#include "shmoo.h"

shmoo::shmoo_result::shmoo_result( const shmoo::shmoo_setup_t& setup )
{
	if( setup.keyword.empty() || (setup.start == -1 && setup.end == -1 && setup.step == -1) ||
		setup.parameters.empty() ||
		setup.shmoo_type == SHMOO_TYPE::UNSPECIFIED ||
		setup.search_mode == SEARCH_MODE::UNSPECIFIED ||
		setup.pattern == nullptr ) {
		throw "Must fill shmoo_setup struct completely before constructing shmoo_result!";
	}

	data.keyword = setup.keyword;
	set_shmoo_range( setup.parameters, setup.start, setup.end, setup.step ); // TODO!!!
	data.shmoo_type = setup.shmoo_type;
	data.search_mode = setup.search_mode;
	data.flag.stop_on_transition = setup.stop_on_transition ? 1 : 0;
	data.flag.ignore_holes = setup.ignore_holes ? 1 : 0;
	data.flag.verbose = setup.verbose ? 1 : 0;
	data.pattern = setup.pattern;

	if( setup.pre_shmoo_hook != nullptr )    set_pre_shmoo_hook( setup.pre_shmoo_hook );
	if( setup.pre_pattern_hook != nullptr )  set_pre_pattern_hook( setup.pre_pattern_hook );
	if( setup.post_pattern_hook != nullptr ) set_post_pattern_hook( setup.post_pattern_hook );
	if( setup.post_shmoo_hook != nullptr )   set_post_shmoo_hook( setup.post_shmoo_hook );
}

void shmoo::shmoo_result::set_shmoo_range( const std::vector<std::vector<shmoo_parameter_t>>& shmoo_parameters, double start, double end, double step )
{
	if( data.shmoo_map.empty() ) {
		size_t num_steps = static_cast<size_t>(floor( (end - start) / step + 1 ));

		// reserve capacity
		data.shmoo_map.reserve( num_steps );
		for( unsigned int dut = 0; dut < MAX_NUM_DUTS; ++dut ) {
			// copy shmoo parameters for each DUT
			data.shmoo_container.at( dut ).shmoo_parameters = shmoo_parameters.at( dut );

			for( shmoo::shmoo_parameters_iterator pi = data.shmoo_container.at( dut ).shmoo_parameters.begin();
				pi != data.shmoo_container.at( dut ).shmoo_parameters.end(); ++pi ) {
				(*pi).shmoo_points.reserve( num_steps );
				(*pi).window.reserve( num_steps );
			}
		}

		// populate shmoo_map
		while( (step > 0 && start <= end) ||
			(step < 0 && start >= end) ) {
			data.shmoo_map.push_back( start );
			start += step;
		}
	}
}

bool shmoo::shmoo_result::validate_shmoo_parameters()
{
	if( data.shmoo_map.empty() ) {
		throw "Shmoo range not specified!";
	}

	if( data.shmoo_type == SHMOO_TYPE::EDGE ) { // edge shmoo can be either linear or binary search
		if( data.search_mode == SEARCH_MODE::BINARY ) {
			data.flag.stop_on_transition = 1; // stop on transition is implied for binary search
		}
	}
	else if( data.shmoo_type == SHMOO_TYPE::WINDOW ) {
		if( data.search_mode == SEARCH_MODE::BINARY ) {
			throw "SHMOO_TYPE::WINDOW cannot use SEARCH_MODE::BINARY!";
		}
		if( data.flag.stop_on_transition ) {
			throw "SHMOO_TYPE::WINDOW cannot be set to stop_on_transition!";
		}
	}

	return true;
}

bool shmoo::shmoo_result::more_shmoo_step()
{
	if( !data.validated ) data.validated = validate_shmoo_parameters(); // validate only once

	for( unsigned int dut = 0; dut < MAX_NUM_DUTS; ++dut ) {
		if( !((data.active_dut_mask >> dut) & 0x1) ) continue;
		
		bool shmoo_next = false;

		for( shmoo_parameters_iterator pi = data.shmoo_container.at( dut ).shmoo_parameters.begin();
			pi != data.shmoo_container.at( dut ).shmoo_parameters.end(); ++pi ) {

			switch( data.search_mode ) {
			case SEARCH_MODE::BINARY:
				shmoo_next = (*pi).binary_search( *this );
				break;
			case SEARCH_MODE::LINEAR:
				shmoo_next = (*pi).linear_search( *this );
				break;
			default:
				fatal( "Unsupported shmoo::SEARCH_MODE!" );
				abort();
				break;
			}
		}

		if( !shmoo_next ) {
			data.active_dut_mask &= ~(0x1 << dut); // remove the dut from active dut mask
		}
	}

	if( data.active_dut_mask ) {
		if( data.active_dut_mask != data.active_dut_mask_prev ) { // only update hardware dut mask as needed
			active_duts_enable( data.active_dut_mask );
		}
		data.active_dut_mask_prev = data.active_dut_mask;
		++data.step;
		return true;
	}
	else { // restore active dut mask
		if( data.active_dut_mask_prev != data.active_dut_mask_save ) {
			active_duts_enable( data.active_dut_mask_save );
		}
		find_window();
		data.step = 0;
		return false;
	}
}

bool shmoo::shmoo_parameter_t::binary_search( shmoo::shmoo_result& base )
{
	bool shmoo_next = false;

	if( base.data.step == 0 ) { // shmoo at least twice, min and max
		shmoo_points.push_back( shmoo_point_t( base.data.shmoo_map.front(), 0 ) );
		shmoo_next = true;
	}
	else if( base.data.step == 1 ) { // shmoo at least twice, min and max
		lower = shmoo_points.back(); // lower initialized
		shmoo_points.push_back( shmoo_point_t( base.data.shmoo_map.back(), static_cast<unsigned int>((base.data.shmoo_map.size() - 1)) ) );
		shmoo_next = true;
	}
	else {
		if( base.data.step == 2 ) {
			upper = shmoo_points.back(); // upper initialized
		}

		const shmoo_point_t& mid = shmoo_points.back();

		//if(1) output("DUT%d lower:%d upper:%d mid:%d", dut, lower.index, upper.index, mid.index);

		unsigned int next_index;
		if( mid.result == lower.result ) { // search forward
			next_index = (mid.index + upper.index) / 2;
			lower = mid;
		}
		else if( mid.result == upper.result ) { // search backward
			next_index = (mid.index + lower.index) / 2;
			upper = mid;
		}

		//if(1) output("DUT%d next:%d", dut, next_index);

		if( next_index == mid.index || next_index == lower.index || next_index == upper.index ) {
			shmoo_next = false;
		}
		else {
			shmoo_points.push_back( shmoo_point_t( base.data.shmoo_map.at( next_index ), next_index ) );
			shmoo_next = true;
		}
	}

	return shmoo_next;
}

bool shmoo::shmoo_parameter_t::linear_search( shmoo::shmoo_result& base )
{
	if( base.data.step >= base.data.shmoo_map.size() ) return false;

	if( base.data.step < 2 ) { // shmoo at least twice
		shmoo_points.push_back( shmoo_point_t( base.data.shmoo_map.at( base.data.step ), base.data.step ) );
		return true;
	}
	else {
		if( base.data.shmoo_type == SHMOO_TYPE::EDGE && base.data.flag.stop_on_transition ) { // shmoo stops on transition
			shmoo_point_t& curr = shmoo_points.back();
			shmoo_point_t& prev = shmoo_points.at( shmoo_points.size() - 2 );
			if( curr.result == prev.result ) { // no transition, continue
				shmoo_points.push_back( shmoo_point_t( base.data.shmoo_map.at( base.data.step ), base.data.step ) );
				return true;
			}
			else { // transition found, stop
				return false;
			}
		}
		else { // shmoo full range
			shmoo_points.push_back( shmoo_point_t( base.data.shmoo_map.at( base.data.step ), base.data.step ) );
			return true;
		}
	}
}

void shmoo::shmoo_result::find_window()
{
	if( data.search_mode == SEARCH_MODE::BINARY || data.flag.stop_on_transition ) return;

	for( unsigned int dut = 0; dut < MAX_NUM_DUTS; ++dut ) {
		if( data.shmoo_container.at( dut ).shmoo_parameters.empty() ) continue; // skip if the DUT is not tested
		for( shmoo_parameters_iterator pi = data.shmoo_container.at( dut ).shmoo_parameters.begin();
			pi != data.shmoo_container.at( dut ).shmoo_parameters.end(); ++pi ) {
			(*pi).find_window( *this );
		}
	}
}

void shmoo::shmoo_parameter_t::find_window( shmoo::shmoo_result& base )
{
	if( base.data.search_mode == SEARCH_MODE::BINARY || base.data.flag.stop_on_transition ) return;

	if( shmoo_points.empty() ) return; // skip if the shmoo parameter has no data

	// try to find the first window and stop on the first found window
	RESULT prev = RESULT::NO_RESULT;
	unsigned short transition_count = 0;
	for( unsigned int index = 0; index < base.data.shmoo_map.size(); ++index ) {
		RESULT& curr = shmoo_points.at( index ).result; // TODO: boundary check

		// skip if no data for this shmoo step, don't update prev
		if( curr == RESULT::NO_RESULT ) continue;

		// first time
		if( prev == RESULT::NO_RESULT ) { prev = curr; continue; }

		// haven't found a new window
		if( (transition_count % 2 == 0) && curr == prev ) { prev = curr; continue; }

		// found the start of a new window
		if( (transition_count % 2 == 0) && curr != prev ) {
			++transition_count;
			window.push_back( shmoo_points.at( index ) );
			if( base.data.shmoo_type == SHMOO_TYPE::EDGE ) break;
			else {
				prev = curr;
				continue;
			}
		}

		// window continues
		if( (transition_count % 2 == 1) && curr == prev ) {
			window.push_back( shmoo_points.at( index ) );
			prev = curr;
			continue;
		}

		// found the end of the window
		if( (transition_count % 2 == 1) && curr != prev ) {
			++transition_count;
			if( base.data.flag.ignore_holes ) {
				prev = curr;
				continue;
			}
			else break;
		}
	}

	// 
	//if(base.data.shmoo_type == SHMOO_TYPE::WINDOW && transition_count == 1) { // a window needs 2 transitions
	//  window.clear();
	//}
}

double shmoo::shmoo_result::current_shmoo_value( unsigned int dut, const std::string& parameter_name )
{
	for( shmoo::shmoo_parameters_iterator pi = data.shmoo_container.at( dut ).shmoo_parameters.begin();
		pi != data.shmoo_container.at( dut ).shmoo_parameters.end(); ++pi ) {
		if( (*pi).name.compare( parameter_name ) != 0 ) continue;
		return (*pi).shmoo_points.back().value;
	}
	return -1;
}

void shmoo::shmoo_result::set_result( unsigned int dut, const std::string& parameter_name, bool result )
{
	for( shmoo::shmoo_parameters_iterator pi = data.shmoo_container.at( dut ).shmoo_parameters.begin();
		pi != data.shmoo_container.at( dut ).shmoo_parameters.end(); ++pi ) {
		if( (*pi).name.compare( parameter_name ) != 0 ) continue;
		(*pi).shmoo_points.back().result = (result ? RESULT::PASS : RESULT::FAIL);
	}
}

double shmoo::shmoo_result::find_value( unsigned dut, const std::string& parameter_name, VALUE_TYPE value_type )
{
	for( shmoo_parameters_iterator pi = data.shmoo_container.at( dut ).shmoo_parameters.begin();
		pi != data.shmoo_container.at( dut ).shmoo_parameters.end(); ++pi ) {
		if( (*pi).name.compare( parameter_name ) != 0 ) continue;

		switch( value_type ) {
		case VALUE_TYPE::TRANSITION:
			return (*pi).find_first_transition( *this );
			break;
		case VALUE_TYPE::WINDOW_START:
			return (*pi).find_first_transition( *this );
			break;
		case VALUE_TYPE::WINDOW_CENTER:
			return (*pi).find_center( *this );
			break;
		case VALUE_TYPE::WINDOW_END:
			return (*pi).find_last_transition( *this );
			break;
		default:
			fatal( "Unsupported shmoo::VALUE_TYPE!" ); abort();
			break;
		}
	}
	return -1;
}

// TODO: what if -1 is a valid shmoo value
double shmoo::shmoo_parameter_t::find_first_transition( shmoo::shmoo_result& base )
{
	if( base.data.search_mode == SEARCH_MODE::BINARY || base.data.flag.stop_on_transition ) {
		if( shmoo_points.empty() ) return -1;
		else return shmoo_points.back().value;
	}
	else {
		if( window.empty() ) return -1;
		else return window.front().value;
	}
}

double shmoo::shmoo_parameter_t::find_last_transition( shmoo::shmoo_result& base )
{
	if( base.data.shmoo_type == SHMOO_TYPE::WINDOW ) {
		if( window.empty() ) return -1;
		else return window.back().value;
	}
	else return -1;
}

double shmoo::shmoo_parameter_t::find_center( shmoo::shmoo_result& base )
{
	if( base.data.shmoo_type == SHMOO_TYPE::WINDOW ) {
		if( window.empty() ) return -1;
		else {
			size_t window_size = window.size();
			return window.at( window_size / 2 ).value;
		}
	}
	else return -1;
}

void shmoo::shmoo_result::execute()
{
	if( data.pre_shmoo_hook != nullptr ) (data.pre_shmoo_hook)();

	while( more_shmoo_step() ) {

		if( data.pre_pattern_hook != nullptr ) (data.pre_pattern_hook)();

		funtest( data.pattern, finish );

		if( data.post_pattern_hook != nullptr ) (data.post_pattern_hook)();
	}

	if( data.post_shmoo_hook != nullptr ) (data.post_shmoo_hook)();
}

void shmoo::shmoo_result::print()
{
	std::stringstream os;

	os << std::setfill( ' ' );

	// print all shmoo values
	os << std::left << std::setw( 26 ) << "Shmoo Step :";
	for( unsigned int index = 0; index < data.shmoo_map.size(); ++index ) {
		os << std::right << std::setw( 6 ) << std::setfill( ' ' ) << index;
	}
	os << std::endl;
	os << std::left << std::setw( 26 ) << "Shmoo Value:";
	for( unsigned int index = 0; index < data.shmoo_map.size(); ++index ) {
		os << std::right << std::setw( 6 ) << data.shmoo_map.at( index );
	}
	os << std::endl;

	// print pass/fail result for each DUT and shmoo parameter
	for( unsigned int dut = 0; dut < MAX_NUM_DUTS; ++dut ) {
		for( shmoo_parameters_iterator pi = data.shmoo_container.at( dut ).shmoo_parameters.begin();
			pi != data.shmoo_container.at( dut ).shmoo_parameters.end(); ++pi ) {

			os << std::left << "DUT" << std::dec << (dut + 1) << " ";
			os << std::setw( 20 ) << (*pi).name << ":";

			for( unsigned int index = 0; index < data.shmoo_map.size(); ++index ) {
				shmoo_points_iterator si;
				for( si = (*pi).shmoo_points.begin(); si != (*pi).shmoo_points.end(); ++si ) {
					if( (*si).index == index ) {
						os << std::right << std::setw( 6 );
						if( (*si).result == RESULT::PASS ) os << "<COLOR=GREEN>     P</COLOR>";
						else if( (*si).result == RESULT::FAIL ) os << "<COLOR=RED>     F</COLOR>";
						break;
					}
				}
				if( si == (*pi).shmoo_points.end() ) {
					os << std::setw( 6 ) << "-";
				}
			}
			os << " ";
			switch( data.shmoo_type ) {
			case SHMOO_TYPE::EDGE:
				os << " FIRST TRANSITION:" << (*pi).find_first_transition( *this );
				break;
			case SHMOO_TYPE::WINDOW:
				os << " WINDOW START:" << (*pi).find_first_transition( *this );
				os << " CENTER:" << (*pi).find_center( *this );
				os << " END:" << (*pi).find_last_transition( *this );
				os << " SIZE:" << std::dec << (*pi).window.size();
				break;
			default:
				break;
			}
			os << std::endl;
		}
	}

	output( os.str().c_str() );
}


