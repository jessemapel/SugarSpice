%module(package="pyspiceql") config

%{
  #include "config.h"
%}

%include "config.h"

%extend SpiceQL::Config {
    SpiceQL::Config __getitem__(std::string ptr) {
        return (*($self))[ptr];
    }
}