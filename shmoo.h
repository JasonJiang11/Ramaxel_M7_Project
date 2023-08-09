#ifndef _SHMOO_H
#define _SHMOO_H

#include <vector>
#include <functional>

/**
  Two Shmoo Types:

  1. Edge Shmoo (find transition point, e.g. write leveling)
	 - Search mode can be either linear or binary
	 - Either search full range or stop on transition
	 - Report transition point

	 Step/Index   0   1   2   3   4   5   6   7
	 Value(PS)   100 125 150 175 200 225 250 275
	 Result       P   P   P   P   F   F   F   F
							  ^   ^
	 find_first_transition  - returns 200 PS
	 find_last_transition   - N/A, error
	 find_center            - N/A, error
	 find_first_of_pattern  - only valid for full range shmoo

  2. Window Shmoo (find passing data eye, e.g. read training)
	 - Search mode must be linear
	 - Always search full range
	 - Report window start/center/end

	 Step/Index   0   1   2   3   4   5   6   7   8   9  10
	 Value(PS)   100 125 150 175 200 225 250 275 300 325 350
	 Result       F   F   F   F   P   P   P   P   P   F   F
							  ^   ^       *       ^   ^
	 find_pre_first_transition - returns 175 PS
	 find_first_transition     - returns 200 PS
	 find_last_transition      - returns 300 PS <-- name?
	 find_post_last_transition - returns 325 PS
	 find_center               - returns 250 PS
	 find_first_of_pattern     - valid
*/

namespace shmoo {
	const unsigned int MAX_NUM_DUTS = 24; // TODO
	enum class SEARCH_MODE {
		UNSPECIFIED = 0,
		BINARY = 1,
		LINEAR = 2
	};
	enum class SHMOO_TYPE {
		UNSPECIFIED = 0,
		EDGE = 1,
		WINDOW = 2
	};
	enum class RESULT {
		NO_RESULT = 0,
		FAIL = 1,
		PASS = 2
	};
	enum class VALUE_TYPE {
		TRANSITION = 0,
		WINDOW_START = 1,
		WINDOW_CENTER = 2,
		WINDOW_END = 3,
		PATTERN = 4
	};

	class shmoo_result;
	using hook = std::function<void()>; // TODO

	// shmoo setup struct
	struct shmoo_parameter_t;
	struct shmoo_setup_t {
		std::string keyword;
		std::vector<std::vector<shmoo_parameter_t>> parameters;
		double start, end, step;
		SHMOO_TYPE shmoo_type;
		SEARCH_MODE search_mode;
		bool stop_on_transition;
		bool ignore_holes;
		bool verbose;
		Pattern* pattern;
		hook pre_shmoo_hook;
		hook pre_pattern_hook;
		hook post_pattern_hook;
		hook post_shmoo_hook;

		shmoo_setup_t() :
			keyword( "" ),
			start( -1 ), end( -1 ), step( -1 ),
			shmoo_type( SHMOO_TYPE::UNSPECIFIED ),
			search_mode( SEARCH_MODE::UNSPECIFIED ),
			stop_on_transition( false ),
			ignore_holes( false ),
			verbose( false ),
			pattern( nullptr ),
			pre_shmoo_hook( nullptr ),
			pre_pattern_hook( nullptr ),
			post_pattern_hook( nullptr ),
			post_shmoo_hook( nullptr ) {
			parameters.resize( MAX_NUM_DUTS );
		}
	};

	// everything for one particular shmoo point
	struct shmoo_point_t {
		double value;
		unsigned int index;
		RESULT result;
		shmoo_point_t() : value( -1 ), index( 0 ), result( RESULT::NO_RESULT ) {}
		shmoo_point_t( double v, unsigned int i ) : value( v ), index( i ), result( RESULT::NO_RESULT ) {}
	};
	using shmoo_points_t        = std::vector<shmoo_point_t>;
	using shmoo_points_iterator = std::vector<shmoo_point_t>::iterator;

	// everything for one particular shmoo parameter
	struct shmoo_parameter_t {
		std::string name;
		PinList* pinlist;
		shmoo_points_t shmoo_points;
		shmoo_points_t window;
		shmoo_point_t lower;
		shmoo_point_t upper;
		shmoo_parameter_t() {}
		shmoo_parameter_t( const std::string& n ) :
			name( n ) {}
		shmoo_parameter_t( const std::string& n, PinList* p ) :
			name( n ),
			pinlist( p ) {}
		bool binary_search( shmoo_result& base );
		bool linear_search( shmoo_result& base );
		void find_window( shmoo_result& base );
		double find_first_transition( shmoo_result& base );
		double find_last_transition( shmoo_result& base );
		double find_center( shmoo_result& base );
	};
	using shmoo_parameters_t        = std::vector<shmoo_parameter_t>;
	using shmoo_parameters_iterator = std::vector<shmoo_parameter_t>::iterator;

	// everything for one DUT
	struct shmoo_container_dut_t {
		shmoo_parameters_t shmoo_parameters;
	};
	using shmoo_container_t        = std::vector<shmoo_container_dut_t>;
	using shmoo_container_iterator = std::vector<shmoo_container_dut_t>::iterator;

	// all data memebers for shmoo_result
	struct shmoo_result_data {
		std::string keyword;
		uint32_t active_dut_mask, active_dut_mask_save, active_dut_mask_prev;
		SHMOO_TYPE shmoo_type;
		SEARCH_MODE search_mode;
		union {
			unsigned int flags;
			struct {
				unsigned int verbose : 1;
				unsigned int stop_on_transition : 1;
				unsigned int ignore_holes : 1;
				unsigned int print_details : 1;
			} flag;
		};
		unsigned int step;
		bool validated;
		std::vector<double> shmoo_map;
		shmoo_container_t shmoo_container;
		Pattern* pattern;

		// hooks
		hook pre_shmoo_hook;
		hook pre_pattern_hook;
		hook post_pattern_hook;
		hook post_shmoo_hook;

		shmoo_result_data() :
			keyword( "" ),
			active_dut_mask( 0 ),
			active_dut_mask_save( 0 ),
			active_dut_mask_prev( 0 ),
			shmoo_type( SHMOO_TYPE::UNSPECIFIED ),
			search_mode( SEARCH_MODE::UNSPECIFIED ),
			flags( 0 ),
			step( 0 ),
			validated( false ),
			pattern( nullptr ),
			pre_shmoo_hook( nullptr ),
			pre_pattern_hook( nullptr ),
			post_pattern_hook( nullptr ),
			post_shmoo_hook( nullptr ) {
			shmoo_container.resize( MAX_NUM_DUTS ); // max_dut()+1 const
		}
	};

	class shmoo_result
	{
		friend struct shmoo_parameter_t;

	public:
		~shmoo_result() {}
		shmoo_result() {}
		shmoo_result( const shmoo_setup_t& setup );

		// set
		void set_active_dut_mask( unsigned int mask ) { data.active_dut_mask = data.active_dut_mask_save = mask; }
		void set_print_details() { data.flag.print_details = 1; }
		void set_verbose() { data.flag.verbose = 1; }
		void set_result( unsigned int dut, const std::string& parameter_name, bool result );

		// hooks
		void set_pre_shmoo_hook( hook f ) { data.pre_shmoo_hook = f; }
		void set_pre_pattern_hook( hook f ) { data.pre_pattern_hook = f; }
		void set_post_pattern_hook( hook f ) { data.post_pattern_hook = f; }
		void set_post_shmoo_hook( hook f ) { data.post_shmoo_hook = f; }

		// get
		const std::string& keyword() const { return data.keyword; }
		double current_shmoo_value( unsigned int dut, const std::string& parameter_name );
		shmoo_parameters_t shmoo_parameters( unsigned int dut ) const { return data.shmoo_container.at( dut ).shmoo_parameters; }

		// find
		double find_value( unsigned int dut, const std::string& parameter_name, VALUE_TYPE value_type = VALUE_TYPE::TRANSITION );

		// 
		void execute();
		bool more_shmoo_step();
		void print();

	private:
		void set_shmoo_range( const std::vector<std::vector<shmoo_parameter_t>>& shmoo_parameters, double start, double end, double step );
		bool validate_shmoo_parameters();
		void find_window();

		// To be deprecated, as shmoo tokens/conditions are to be initialized by shmoo_setup struct
		void set_keyword( const std::string& keyword ) { data.keyword = keyword; }
		void set_shmoo_type( SHMOO_TYPE type ) { data.shmoo_type = type; }
		void set_search_mode( SEARCH_MODE mode ) { data.search_mode = mode; }
		void set_stop_on_transition( bool stop_on_transition ) { data.flag.stop_on_transition = stop_on_transition ? 1 : 0; }
		void set_pattern( Pattern* pattern ) { data.pattern = pattern; }

		shmoo_result_data data;
	};
	using shmoo_result_map = std::map<std::string, shmoo_result>;
}

#endif
