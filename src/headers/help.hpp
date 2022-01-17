/*
 * =====================================================================================
 *
 *       Filename:  help.hpp
 *    Description:  Help display 
 *         Author:  DAOUDI Idriss
 *   Organization:  INRIA
 *
 * =====================================================================================
 */

inline void Help()
{
    std::cout << " \n"
                 "***************************************************************** \n"
                 " \n"
                 "*** Usage : \n"
                 "   somp path/to/trace/file.rec path/to/plaform/file.xml [options] \n"
                 " \n"
                 "*** Mandatory options: \n"
                 "   -m : Matrix size (m*m matrix)\n"
                 "   -b : Bloc/tile size (b*b tile)\n"
                 "   -t : Number of threads/cores\n"
                 "   -o : Model options:\n"
                 "\t\t* nomodel: no model is used by the simulator\n"
                 "\t\t* comm   : communications based model\n"
                 "\t\t* gemm   : GEMM based model\n"
                 "   -a : Thread affinity options:\n"
                 "\t\t* close: equivalent to OMP_PROC_BIND=close\n"
                 "\t\t* spread: equivalent to OMP_PROC_BIND=spread\n"
                 "   -h : Display this help\n"
                 " \n"
                 "***************************************************************** \n"
                 " \n";
}
