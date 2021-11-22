%module pyspiceql

%include std_string.i
%include std_vector.i

// namespace std {
//     %template(VecDouble) vector<double>;
//     %template(VecVecDouble) vector< vector<double> >;
// }

%include "io.i"
%include "query.i"
%include "spice_types.i"
%include "utils.i"