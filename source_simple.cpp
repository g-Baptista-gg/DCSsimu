/*
 * Source_simple.cpp
 *
 *  Created on: Oct 31, 2020
 *      Author: danip
 */

#include "source_simple.hh"


using namespace std;


extern Geolengthelements Geolengthelements;
extern GeoParameters GeoParameters;
extern GeoParapathlengths GeoParapathlengths;
extern plotparameters plotparameters;
extern Graph_options Graph_options;
extern temperature_parameters temperature_parameters;
extern numberrays numberrays;
extern UserSettings UserSettings;
extern fullenergyspectrum fullenergyspectrum;

extern double teta_crys1, Mini_angl, Maxi_angl, d_lat;

extern bool export_prof;

extern ofstream hist_para, hist_anti;

extern double peak_posi_para, peak_posi_anti;

extern double max_plot[2][2], min_plot[2][2];

void Source_simple::run_Source(){

    double max_valu_para, max_valu_anti, S_aper_R_2, z_max, z_min, y_max, y_min, tetaref, cos_e, cos2_e, sin_e, tan_e;
    double n1x, n1y, n1z, Dis_total, teta_max, teta_min, squa_tilt1, squa_tilt2, twtilt_C1, delrot_min, delrot_max, delrot_inc;
    double delrot, d_lat1, d_lat2, tw_d1, tw_d2, a_lamds_uni, b_lamds_uni, del_teta, cosdel, fi_min, fi_max, del_fi, abs_fi_max;
    double cosdel_othe, cosdel_teta, cosdel_teta_othe, n2x_para, n2y_para, n2x_anti, n2y_anti, n2z, p, tetadir, fidir, z, y, r, tetap;
    double sin_fi, cos_fi, sin_tetadir, cos_tetadir, rx, ry, rz, angle, lamda, tetabra1, tetabra2, sin_teref_tedi, toint_para_nor, toint_anti_nor;
    double angle_para, angle_anti, amplitu_con_para, amplitu_con_anti, r2x, r2y, r2z, r3x, r3y, r3z;

    vector<int> int_time_out;
    int int_time_out_begg, int_time_mili_out_begg;

    int numbins, max_para, I, toint_para, toint_anti;

    bool sec_crystal_Parallel_reach, sec_crystal_Antiparallel_reach, reach, first_crystal_reach;

    sec_crystal_Parallel_reach = false;
    sec_crystal_Antiparallel_reach = false;
    peak_posi_para = 0;
    peak_posi_anti = 0;
    max_valu_para = 0;
    max_valu_anti = 0;

    S_aper_R_2 = Geolengthelements.S_aper / 2;
    z_max = Geolengthelements.zdetc / 2;
    z_min = -Geolengthelements.zdetc / 2;
    y_max = Geolengthelements.ydetc / 2;
    y_min = -Geolengthelements.ydetc / 2;

    tetaref = M_PI / 2 - teta_crys1;
    cos_e = cos(tetaref);
    cos2_e = cos(2 * tetaref);
    sin_e = sin(tetaref);
    tan_e = tan(tetaref) / 2;

    n1x = cos(GeoParameters.tilt_C1) * sin_e;
    n1y = cos(GeoParameters.tilt_C1) * cos_e;
    n1z = sin(GeoParameters.tilt_C1);


    Dis_total = GeoParapathlengths.LT_aper + GeoParapathlengths.dist_T_Cr1 + GeoParapathlengths.dist_Cr1_Cr2 + GeoParapathlengths.dist_Cr2_Det;
    teta_max = atan((Geolengthelements.S_aper + Geolengthelements.ydetc) / (2 * Dis_total));
    teta_min = -atan((Geolengthelements.S_aper + Geolengthelements.ydetc) / (2 * Dis_total));

    squa_tilt1 = 0.5 * pow(GeoParameters.tilt_C1, 2);
    squa_tilt2 = 0.5 * pow(GeoParameters.tilt_C2, 2);
    twtilt_C1 = 2 * GeoParameters.tilt_C1;

    delrot_min = Mini_angl * M_PI / 180;
    delrot_max = Maxi_angl * M_PI / 180;
    delrot_inc = (delrot_max - delrot_min) / (double)plotparameters.nubins;

    numbins = 0;

    delrot = delrot_max;

    min_plot[0][0] = Mini_angl + (teta_crys1 * 180 / M_PI);
    min_plot[1][0] = Mini_angl - (teta_crys1 * 180 / M_PI);
    max_plot[0][0] = Maxi_angl + (teta_crys1 * 180 / M_PI);
    max_plot[1][0] = Maxi_angl - (teta_crys1 * 180 / M_PI);

    max_para = 0;


    if(Graph_options.make_graph_profile){
        //TODO
        //start gui ?
        //its probably linked to the gui portion on the main file
        //program later
    }


    d_lat1 = Latice_temp(d_lat, temperature_parameters.T_crystal_1_para);
    d_lat2 = Latice_temp(d_lat, temperature_parameters.T_crystal_2_para);

    tw_d1 = 2 * d_lat1;
    tw_d2 = 2 * d_lat2;


    vector<double> res = Obtain_misalign::misalign(Dis_total);
    fi_max = res[0];
    fi_min = res[1];
    z_max = res[2];
    z_min = res[3];

    del_fi = fi_max - fi_min;

    if(abs(fi_max) > abs(fi_min))
        abs_fi_max = abs(fi_max);
    else
        abs_fi_max = abs(fi_min);


    if(tw_d1 < tw_d2){
        a_lamds_uni = tw_d1 * sin(tetaref + delrot_min);
        b_lamds_uni = tw_d2 * sin(tetaref + delrot_max);
    }else{
        a_lamds_uni = tw_d2 * sin(tetaref + delrot_min);
        b_lamds_uni = tw_d1 * sin(tetaref + delrot_max);
    }

    b_lamds_uni -= a_lamds_uni;


    vector<double> lims = Limits_horizontal_diverge::getLims(tetaref, delrot_min, delrot_max, fi_max, teta_max, teta_min);

    teta_min = lims[0];
    teta_min = lims[1];

    del_teta = teta_max - teta_min;

    int_time_out.push_back(0);
    int_time_out.push_back(0);
    int_time_out = Obtain_time::simuTime(0, 0, int_time_out[0], int_time_out[1]);
    int_time_out_begg = int_time_out[0];
    int_time_mili_out_begg = int_time_out[1];


    while(numbins <= plotparameters.nubins){
        numbins++;

        toint_para = 0;
        toint_anti = 0;

        cosdel = (GeoParameters.tilt_C2 + twtilt_C1 * cos(delrot));
        cosdel_othe = 2 * GeoParameters.tilt_C1 * (GeoParameters.tilt_C2 + GeoParameters.tilt_C1 * cos(delrot));
        cosdel_teta = (GeoParameters.tilt_C2 - twtilt_C1 * cos(delrot + 2 * tetaref));
        cosdel_teta_othe = 2 * GeoParameters.tilt_C1 * (GeoParameters.tilt_C2 - GeoParameters.tilt_C1 * cos(delrot + 2 * tetaref));

        n2x_para = cos(GeoParameters.tilt_C2) * sin(tetaref + delrot + M_PI);
        n2y_para = cos(GeoParameters.tilt_C2) * cos(tetaref + delrot + M_PI);
        n2x_anti = cos(GeoParameters.tilt_C2) * sin(3 * tetaref + delrot);
        n2y_anti = cos(GeoParameters.tilt_C2) * cos(3 * tetaref + delrot);
        n2z = sin(GeoParameters.tilt_C2);

        I = 1;

        while(I <= numberrays.nbeams){
            p = 2 * del_teta * ((double)rand() / RAND_MAX) + teta_min;

            tetadir = p;

            if(UserSettings.Make_Vertical){
                p = ((double)rand() / RAND_MAX) * del_fi + fi_min;
                fidir = p;
            }else
                fidir = GeoParameters.xsi;

            if(GeoParapathlengths.type_source == "P"){
                z = 0;
                y = 0;
            }else if(GeoParapathlengths.type_source == "U"){
                r = ((double)rand() / RAND_MAX) * S_aper_R_2;
                p = 2 * M_PI * ((double)rand() / RAND_MAX);
                tetap = p;
                z = r * cos(tetap);
                y = r * sin(tetap);
            }else{
                z = 0;
                y = 0;

                cout << "Simple simulation only for point or uniform source, proceding as point source" << endl;
            }


            reach = Reached(z, y, tetadir, fidir, Dis_total, z_max, z_min, y_max, y_min);

            if(reach){
                if(UserSettings.angle_aprox == 0){
                    sin_fi = sin(fidir);
                    cos_fi = cos(fidir);
                    sin_tetadir = sin(tetadir);
                    cos_tetadir = cos(tetadir);
                    rx = -cos_fi * cos_tetadir;
                    ry = -cos_fi * sin_tetadir;
                    rz = -sin_fi;

                    vector<double> angleRes;
                    angleRes = getFullAngle(rx, ry, rz, n1x, n1y, n1z);
                    angle = angleRes[0];
                    r2x = angleRes[1];
                    r2y = angleRes[2];
                    r2z = angleRes[3];
                }else if(UserSettings.angle_aprox == 1){
                    sin_fi = sin(fidir);
                    cos_fi = cos(fidir);

                    angle = getFirstApproxAngle(tetaref, tetadir, sin_fi, cos_fi, GeoParameters.tilt_C1, squa_tilt1);
                }else if(UserSettings.angle_aprox == 2){
                    angle = getFullApproximationAngle(tetaref, tetadir, cos_e, tan_e, fidir, GeoParameters.tilt_C1);
                }else{
                    throw runtime_error("Error in angle_aprox: must be 0, 1 or 2, given was " + to_string(UserSettings.angle_aprox));
                }

                if(fullenergyspectrum.make_more_lines < 2)
                    lamda = IntensitySource::getEnergy(a_lamds_uni, b_lamds_uni, tw_d1);
                else
                    throw runtime_error("Error in intensity_source: for a simple source the energy generation cannot be read from file");

                tetabra1 = asin(lamda / tw_d1);

                first_crystal_reach = Gaussi_rockin::getReflection(angle, tetabra1, lamda, false, false);


                if(first_crystal_reach){
                    tetabra2 = asin(lamda / tw_d2);

                    if(UserSettings.angle_aprox == 0){
                        if(UserSettings.see_para){
                            vector<double> angleRes;
                            angleRes = getFullAngle2(r2x, r2y, r2z, n2x_para, n2y_para, n2z);
                            angle = angleRes[0];
                            r3x = angleRes[1];
                            r3y = angleRes[2];
                            r3z = angleRes[3];

                            sec_crystal_Parallel_reach = Gaussi_rockin::getReflection(angle, tetabra2, lamda, true, false);
                        }

                        if(UserSettings.see_anti){
                            vector<double> angleRes;
                            angleRes = getFullAngle2(r2x, r2y, r2z, n2x_anti, n2y_anti, n2z);
                            angle = angleRes[0];
                            r3x = angleRes[1];
                            r3y = angleRes[2];
                            r3z = angleRes[3];

                            sec_crystal_Parallel_reach = Gaussi_rockin::getReflection(angle, tetabra2, lamda, true, false);
                        }

                    }else if(UserSettings.angle_aprox == 1){
                        sin_teref_tedi = sin(tetadir + tetaref);

                        if(UserSettings.see_para){
                            angle = getFirstApproxAngle2(tetaref, tetadir, delrot, sin_fi, cos_fi, squa_tilt2, cosdel, cosdel_othe, cosdel_teta, cosdel_teta_othe, sin_teref_tedi, true);

                            sec_crystal_Parallel_reach = Gaussi_rockin::getReflection(angle, tetabra2, lamda, true, false);
                        }

                        if(UserSettings.see_anti){
                            angle = getFirstApproxAngle2(tetaref, tetadir, delrot, sin_fi, cos_fi, squa_tilt2, cosdel, cosdel_othe, cosdel_teta, cosdel_teta_othe, sin_teref_tedi, false);

                            sec_crystal_Antiparallel_reach = Gaussi_rockin::getReflection(angle, tetabra2, lamda, true, false);
                        }
                    }else if(UserSettings.angle_aprox == 2){
                        if(UserSettings.see_para){
                            angle = getFullApproximationAngle2(tetaref, tetadir, delrot, cos_e, tan_e, cos2_e, fidir, GeoParameters.tilt_C1, GeoParameters.tilt_C2, true);

                            sec_crystal_Parallel_reach = Gaussi_rockin::getReflection(angle, tetabra2, lamda, true, false);
                        }

                        if(UserSettings.see_anti){
                            angle = getFullApproximationAngle2(tetaref, tetadir, delrot, cos_e, tan_e, cos2_e, fidir, GeoParameters.tilt_C1, GeoParameters.tilt_C2, false);

                            sec_crystal_Antiparallel_reach = Gaussi_rockin::getReflection(angle, tetabra2, lamda, true, false);
                        }

                    }else
                        throw runtime_error("Error in angle_aprox: must be 0, 1 or 2, given was " + to_string(UserSettings.angle_aprox));


                    if(sec_crystal_Parallel_reach or sec_crystal_Antiparallel_reach){
                        toint_para++;
                        toint_anti++;
                    }

                }

            }


            I++;
        }


        toint_para_nor = (double)toint_para / numberrays.nbeams;
        toint_anti_nor = (double)toint_anti / numberrays.nbeams;

        angle_para = (delrot * 180 / M_PI) + (teta_crys1 * 180 / M_PI);
        angle_anti = (delrot * 180 / M_PI) - (teta_crys1 * 180 / M_PI);


        if(export_prof){
            hist_para << -angle_para << "\t" << toint_para << "\t" << sqrt((double)toint_para) << endl;
            hist_anti << -angle_anti << "\t" << toint_anti << "\t" << sqrt((double)toint_anti) << endl;
        }

        if(Graph_options.make_graph_profile){
            //TODO
            //implement gui plotting
        }

        if(max_para < toint_para){
            max_para = toint_para;
        }


        if(UserSettings.fitting){
            Obtain_data_fitting::FitData(numbins, angle_para, toint_para, angle_anti, toint_anti);

            if(max_valu_para < toint_para){
                max_valu_para = toint_para;
                peak_posi_para = angle_para;
                amplitu_con_para = (double)toint_para;
            }

            if(max_valu_anti < toint_anti){
                max_valu_anti = toint_anti;
                peak_posi_anti = angle_anti;
                amplitu_con_anti = (double)toint_anti;
            }

        }

        int_time_out = Obtain_time::simuTime(1, plotparameters.nubins - numbins, int_time_out[0], int_time_out[1]);

        cout << numbins << " of " << plotparameters.nubins << " done" << endl;
        delrot = delrot - delrot_inc;
    }

    int_time_out = Obtain_time::simuTime(1, plotparameters.nubins - numbins, int_time_out_begg, int_time_mili_out_begg);

    if(Graph_options.make_graph_profile){
        //TODO
        //implement gui
    }

}
