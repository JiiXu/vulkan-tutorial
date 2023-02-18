#pragma once

#include <string>
#include <vector>

namespace lve {
  class Pipeline {
    private:

      static std::vector< char > readFile( const std::string& );

      void createGraphicsPipeline( const std::string&, const std::string& );

    public:

      Pipeline( const std::string&, const std::string& );
  };
}

