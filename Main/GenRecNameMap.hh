#ifndef GENRECNAMEMAP
#define GENRECNAMEMAP

#include "Tools/MConfig.hh"
#include "Tools/Tools.hh"

class GenRecNameMap {
   public:
      struct GenRecNamePair {
         GenRecNamePair( std::string const genName = "",
                         std::string const recName = ""
                         ) :
            GenName( genName ),
            RecName( recName )
         {}

         std::string GenName;
         std::string RecName;
      };

      typedef std::map< std::string, GenRecNamePair > NameMap;
      typedef NameMap::const_iterator const_iterator;

      GenRecNameMap( Tools::MConfig const &cfg ) :
         m_muo_type_gen( cfg.GetItem< std::string >( "Muon.Type.Gen" ) ),
         m_muo_type_rec( cfg.GetItem< std::string >( "Muon.Type.Rec" ) ),

         m_ele_type_gen( cfg.GetItem< std::string >( "Ele.Type.Gen" ) ),
         m_ele_type_rec( cfg.GetItem< std::string >( "Ele.Type.Rec" ) ),

         m_tau_type_gen( cfg.GetItem< std::string >( "Tau.Type.Gen" ) ),
         m_tau_type_rec( cfg.GetItem< std::string >( "Tau.Type.Rec" ) ),

         m_gam_type_gen( cfg.GetItem< std::string >( "Gamma.Type.Gen" ) ),
         m_gam_type_rec( cfg.GetItem< std::string >( "Gamma.Type.Rec" ) ),

         m_jet_type_gen( cfg.GetItem< std::string >( "Jet.Type.Gen" ) ),
         m_jet_type_rec( cfg.GetItem< std::string >( "Jet.Type.Rec" ) ),

         m_met_type_gen( cfg.GetItem< std::string >( "MET.Type.Gen" ) ),
         m_met_type_rec( cfg.GetItem< std::string >( "MET.Type.Rec" ) ),

         m_gen_rec_map( initNameMap() )
      {}

      GenRecNamePair const &get( std::string const &object ) const {
         NameMap::const_iterator it = m_gen_rec_map.find( object );

         if( it == m_gen_rec_map.end() ) {
            std::string const err = "No name mapping found for object '" + object + "'!";
            throw Tools::config_error( err );
         } else {
            return (*it).second;
         }
      }

      const_iterator begin() const { return m_gen_rec_map.begin(); }
      const_iterator end() const { return m_gen_rec_map.end(); }

   private:
      NameMap const initNameMap() const {
         NameMap GenRecMap;
         GenRecMap[ "Muo" ] = GenRecNamePair( m_muo_type_gen, m_muo_type_rec );
         GenRecMap[ "Ele" ] = GenRecNamePair( m_ele_type_gen, m_ele_type_rec );
         GenRecMap[ "Tau" ] = GenRecNamePair( m_tau_type_gen, m_tau_type_rec );
         GenRecMap[ "Gam" ] = GenRecNamePair( m_gam_type_gen, m_gam_type_rec );
         GenRecMap[ "Jet" ] = GenRecNamePair( m_jet_type_gen, m_jet_type_rec );
         GenRecMap[ "MET" ] = GenRecNamePair( m_met_type_gen, m_met_type_rec );

         return GenRecMap;
      }

      std::string const m_muo_type_gen;
      std::string const m_muo_type_rec;

      std::string const m_ele_type_gen;
      std::string const m_ele_type_rec;

      std::string const m_tau_type_gen;
      std::string const m_tau_type_rec;

      std::string const m_gam_type_gen;
      std::string const m_gam_type_rec;

      std::string const m_jet_type_gen;
      std::string const m_jet_type_rec;

      std::string const m_met_type_gen;
      std::string const m_met_type_rec;

      NameMap const m_gen_rec_map;
};

#endif /*GENRECNAMEMAP*/
