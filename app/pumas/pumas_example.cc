#include "pumas/PUMASInterface.hh"
/* Standard library includes */
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "pumas/pumas.h"
using namespace PUMASInterface;

int main(int narg, char * argv[]){

    /* Parse the arguments */
    double rock_thickness = strtod(argv[1], NULL);

    const double elevation = strtod(argv[2], NULL);
    const double kinetic_min = strtod(argv[3], NULL);
    const double kinetic_max =
        (narg >= 5) ? strtod(argv[4], NULL) : kinetic_min;


    std::cout << "Got Inputs : " << std::endl;

    pumas_context* context = PUMASInterface::BuildPUMAS();
    std::cout << "Built context : " << context << std::endl;



    /* Run the Monte-Carlo */
    const double cos_theta = cos((90. - elevation) / 180. * M_PI);
    const double sin_theta = sqrt(1. - cos_theta * cos_theta);
    const double rk = log(kinetic_max / kinetic_min);
    double w = 0., w2 = 0.;
    const int n = 20;
    int i;
    for (i = 0; i < n; i++) {
        std::cout << "Throwing" << std::endl;
        /* Set the muon final state */
        double kf, wf;
        if (rk) {
            /* The final state kinetic energy is randomised over
             * a log-uniform distribution. The Monte-Carlo weight is
             * initialised according to this generating bias PDF,
             * i.e. wf = 1 / PDF(kf).
             */
            kf = kinetic_min * exp(rk * PUMASInterface::uniform01(context));
            wf = kf * rk;
        } else {
            /* A point estimate is computed, for a fixed final
             * state energy.
             */
            kf = kinetic_min;
            wf = 1;
        }
        std::cout << "Creating state" << std::endl;
        struct pumas_state state = {.charge = -1.,
                   .kinetic = kf,
                    .weight = wf,
                     .direction = { -sin_theta, 0., -cos_theta }
        };

        /* Transport the muon backwards */
        const double kinetic_threshold = kinetic_max * 1E+03;
        std::cout << "Transporting " << state.kinetic << " < " << kinetic_max * 1E+03-FLT_EPSILON << std::endl;
        int count = 0;
        while (state.kinetic < kinetic_threshold - FLT_EPSILON) {
            std::cout << "While " << state.position[0] << " " << state.position[1] << " " << state.position[2] <<  std::endl;
            count++;
            if (state.kinetic < 1E+02 - FLT_EPSILON) {
                /* Below 100 GeV do a detailed simulation
                 * à la Geant4, including transverse transport
                 */
                context->scheme = PUMAS_SCHEME_HYBRID;
                context->longitudinal = 0;
                context->kinetic_limit = 1E+02;
            } else {
                /* Do a fast simulation à la MUM */
                context->scheme = PUMAS_SCHEME_HYBRID;
                context->longitudinal = 0;
                context->kinetic_limit = kinetic_threshold;
            }
            std::cout << "Calling transport API" << std::endl;
            pumas_transport(context, &state);

            double mediumval = medium2(context, &state, NULL);
            std::cout << count << " Checking for exit " << mediumval << " "  << state.kinetic << std::endl;
            std::cout << count << " New Pos " << state.position[0] << " " << state.position[1] << " " << state.position[2] <<  std::endl;

            /* Check if the muon has exit the simulation area */
            if (mediumval <= 0.) break;
        }

        /* Update the integrated flux */
        const double wi = state.weight *
                          spectrum_gaisser(-state.direction[2], state.kinetic);
        w += wi;
        w2 += wi * wi;
    }

    /* Print the (integrated) flux */
    w /= n;
    const double sigma =
        (rock_thickness <= 0.) ? 0. : sqrt(((w2 / n) - w * w) / n);
    printf("Integrated flux : %.5lE \\pm %.5lE m^{-2} s^{-2} sr^{-1}\n", w,
           sigma);


}

