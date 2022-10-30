#include "wilson.h"

void wilson_dslash_kernel(
    double (*chi_p)[LZ][LY][LX / 2][4][3][2],
    double (*u_p_f_tst)[LT + 1][LZ + 1][LY + 1][(LX / 2) + 1][4][3][3][2],
    double (*psi_p_f_tst)[LZ + 2][LY + 2][(LX / 2) + 2][4][3][2], int cb,
    FILE *fp) {
  struct timeval tv1, tv2;
  gettimeofday(&tv1, NULL);
  long start = (tv1.tv_sec * 1000000 + tv1.tv_usec);
#pragma omp target enter data map(                                             \
    to                                                                         \
    : chi_p [0:64] [0:16] [0:48] [0:16] [0:4] [0:3] [0:2],                     \
      u_p_f_tst [0:2] [0:65] [0:17] [0:49] [0:17] [0:4] [0:3] [0:3] [0:2],     \
      psi_p_f_tst [0:66] [0:18] [0:50] [0:18] [0:4] [0:3] [0:2])

#pragma omp target teams distribute collapse(3)
  for (int t = 1; t < LT + 1; t++) {
    for (int z = 1; z < LZ + 1; z++) {
      for (int y = 1; y < LY + 1; y++) {
#pragma omp parallel for
        for (int x = 2; x < LX + 2; x++) {
          double chi_tmp[24];
          double tmp_tst[4][3][2];
          int cbn = (cb == 0 ? 1 : 0);
          int parity = (x - 2) + (y - 1) + (z - 1) + (t - 1);
          int sdag = 1;

          parity = parity % 2;
          if (parity != cb) {
            /* x,y,z,t addressing of cb checkerboard */
            int xp = (x + 1);
            int yp = (y + 1);
            int zp = (z + 1);
            int tp = (t + 1);

            int xm = x - 1;
            int ym = y - 1;
            int zm = z - 1;
            int tm = t - 1;

            /* 1-gamma_0 */
            /*-----------*/

            int mu = 0;
            for (int c = 0; c < 3; c++) {
              tmp_tst[0][c][0] = psi_p_f_tst[t][z][y][xp / 2][0][c][0] +
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][3][c][1]);

              tmp_tst[0][c][1] = psi_p_f_tst[t][z][y][xp / 2][0][c][1] -
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][3][c][0]);

              tmp_tst[1][c][0] = psi_p_f_tst[t][z][y][xp / 2][1][c][0] +
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][2][c][1]);

              tmp_tst[1][c][1] = psi_p_f_tst[t][z][y][xp / 2][1][c][1] -
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][2][c][0]);

              tmp_tst[2][c][0] = psi_p_f_tst[t][z][y][xp / 2][2][c][0] -
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][1][c][1]);

              tmp_tst[2][c][1] = psi_p_f_tst[t][z][y][xp / 2][2][c][1] +
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][1][c][0]);

              tmp_tst[3][c][0] = psi_p_f_tst[t][z][y][xp / 2][3][c][0] -
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][0][c][1]);

              tmp_tst[3][c][1] = psi_p_f_tst[t][z][y][xp / 2][3][c][1] +
                                 sdag * (psi_p_f_tst[t][z][y][xp / 2][0][c][0]);
            }

            /* multiply by U_mu */
            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                chi_tmp[c * 2 + s * 3 * 2] =
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][0]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][1]);

                chi_tmp[1 + c * 2 + s * 3 * 2] =
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][0]);
              }
            }

            /* 1-gamma_1 */
            /*-----------*/

            for (int c = 0; c < 3; c++) {
              // tmp[0 + c*2 + 0*2*3]
              tmp_tst[0][c][0] =
                  psi_p_f_tst[t][z][yp][x / 2][0][c][0] -
                  sdag * (-psi_p_f_tst[t][z][yp][x / 2][3][c][0]);

              // tmp[1 + c*2 + 0*2*3]
              tmp_tst[0][c][1] =
                  psi_p_f_tst[t][z][yp][x / 2][0][c][1] -
                  sdag * (-psi_p_f_tst[t][z][yp][x / 2][3][c][1]);

              // tmp[0 + c*2 + 1*2*3]
              tmp_tst[1][c][0] = psi_p_f_tst[t][z][yp][x / 2][1][c][0] -
                                 sdag * (psi_p_f_tst[t][z][yp][x / 2][2][c][0]);

              // tmp[1 + c*2 + 1*2*3]
              tmp_tst[1][c][1] = psi_p_f_tst[t][z][yp][x / 2][1][c][1] -
                                 sdag * (psi_p_f_tst[t][z][yp][x / 2][2][c][1]);

              // tmp[0 + c*2 + 2*2*3]
              tmp_tst[2][c][0] = psi_p_f_tst[t][z][yp][x / 2][2][c][0] -
                                 sdag * (psi_p_f_tst[t][z][yp][x / 2][1][c][0]);

              // tmp[1 + c*2 + 2*2*3]
              tmp_tst[2][c][1] = psi_p_f_tst[t][z][yp][x / 2][2][c][1] -
                                 sdag * (psi_p_f_tst[t][z][yp][x / 2][1][c][1]);

              // tmp[0 + c*2 + 3*2*3]
              tmp_tst[3][c][0] =
                  psi_p_f_tst[t][z][yp][x / 2][3][c][0] -
                  sdag * (-psi_p_f_tst[t][z][yp][x / 2][0][c][0]);

              // tmp[1 + c*2 + 3*2*3]
              tmp_tst[3][c][1] =
                  psi_p_f_tst[t][z][yp][x / 2][3][c][1] -
                  sdag * (-psi_p_f_tst[t][z][yp][x / 2][0][c][1]);
            }

            /* multiply by U_mu */
            mu = 1;

            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][0]
                chi_tmp[c * 2 + s * 3 * 2] +=
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][0]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][1]);

                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][1]
                chi_tmp[c * 2 + s * 3 * 2 + 1] +=
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][0]);
              }
            }

            /* 1-gamma_2 */
            /*-----------*/

            for (int c = 0; c < 3; c++) {
              // TMP(0,c,0)
              tmp_tst[0][c][0] =
                  psi_p_f_tst[t][zp][y][x / 2][0][c][0] -
                  sdag * (-psi_p_f_tst[t][zp][y][x / 2][2][c][1]);

              // TMP(1,c,0)
              tmp_tst[0][c][1] = psi_p_f_tst[t][zp][y][x / 2][0][c][1] -
                                 sdag * (psi_p_f_tst[t][zp][y][x / 2][2][c][0]);

              // TMP(0,c,1)
              tmp_tst[1][c][0] = psi_p_f_tst[t][zp][y][x / 2][1][c][0] -
                                 sdag * (psi_p_f_tst[t][zp][y][x / 2][3][c][1]);

              // TMP(1,c,1)
              tmp_tst[1][c][1] =
                  psi_p_f_tst[t][zp][y][x / 2][1][c][1] -
                  sdag * (-psi_p_f_tst[t][zp][y][x / 2][3][c][0]);

              // TMP(0,c,2)
              tmp_tst[2][c][0] = psi_p_f_tst[t][zp][y][x / 2][2][c][0] -
                                 sdag * (psi_p_f_tst[t][zp][y][x / 2][0][c][1]);

              // TMP(1,c,2)
              tmp_tst[2][c][1] =
                  psi_p_f_tst[t][zp][y][x / 2][2][c][1] -
                  sdag * (-psi_p_f_tst[t][zp][y][x / 2][0][c][0]);

              // TMP(0,c,3)
              tmp_tst[3][c][0] =
                  psi_p_f_tst[t][zp][y][x / 2][3][c][0] -
                  sdag * (-psi_p_f_tst[t][zp][y][x / 2][1][c][1]);

              // TMP(1,c,3)
              tmp_tst[3][c][1] = psi_p_f_tst[t][zp][y][x / 2][3][c][1] -
                                 sdag * (psi_p_f_tst[t][zp][y][x / 2][1][c][0]);
            }

            /* multiply by U_mu */
            mu = 2;

            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][0]
                chi_tmp[c * 2 + s * 3 * 2] +=
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][0]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][1]);

                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][1]
                chi_tmp[c * 2 + s * 3 * 2 + 1] +=
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][0]);
              }
            }

            /* 1-gamma_3 */
            /*-----------*/

            for (int c = 0; c < 3; c++) {
              // TMP(0,c,0)
              tmp_tst[0][c][0] = psi_p_f_tst[tp][z][y][x / 2][0][c][0] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][2][c][0]);

              // TMP(1,c,0)
              tmp_tst[0][c][1] = psi_p_f_tst[tp][z][y][x / 2][0][c][1] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][2][c][1]);

              // TMP(0,c,1)
              tmp_tst[1][c][0] = psi_p_f_tst[tp][z][y][x / 2][1][c][0] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][3][c][0]);

              // TMP(1,c,1)
              tmp_tst[1][c][1] = psi_p_f_tst[tp][z][y][x / 2][1][c][1] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][3][c][1]);

              // TMP(0,c,2)
              tmp_tst[2][c][0] = psi_p_f_tst[tp][z][y][x / 2][2][c][0] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][0][c][0]);

              // TMP(1,c,2)
              tmp_tst[2][c][1] = psi_p_f_tst[tp][z][y][x / 2][2][c][1] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][0][c][1]);

              // TMP(0,c,3)
              tmp_tst[3][c][0] = psi_p_f_tst[tp][z][y][x / 2][3][c][0] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][1][c][0]);

              // TMP(1,c,3)
              tmp_tst[3][c][1] = psi_p_f_tst[tp][z][y][x / 2][3][c][1] -
                                 sdag * (psi_p_f_tst[tp][z][y][x / 2][1][c][1]);
            }

            /* multiply by U_mu */
            mu = 3;

            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][0]
                chi_tmp[c * 2 + s * 3 * 2] +=
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][0]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][1]

                     - u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][1]);

                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][1]
                chi_tmp[c * 2 + s * 3 * 2 + 1] +=
                    (u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][0] *
                         tmp_tst[s][0][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][0] *
                           tmp_tst[s][1][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][0] *
                           tmp_tst[s][2][1]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][0][c][1] *
                           tmp_tst[s][0][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][1][c][1] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cbn][t][z][y][x / 2][mu][2][c][1] *
                           tmp_tst[s][2][0]);
              }
            }

            /* 1+gamma_0 */
            /*-----------*/

            for (int c = 0; c < 3; c++) {
              // TMP(0,c,0)
              tmp_tst[0][c][0] =
                  psi_p_f_tst[t][z][y][xm / 2][0][c][0] +
                  sdag * (-psi_p_f_tst[t][z][y][xm / 2][3][c][1]);

              // TMP(1,c,0)
              tmp_tst[0][c][1] = psi_p_f_tst[t][z][y][xm / 2][0][c][1] +
                                 sdag * (psi_p_f_tst[t][z][y][xm / 2][3][c][0]);

              // TMP(0,c,1)
              tmp_tst[1][c][0] =
                  psi_p_f_tst[t][z][y][xm / 2][1][c][0] +
                  sdag * (-psi_p_f_tst[t][z][y][xm / 2][2][c][1]);

              // TMP(1,c,1)
              tmp_tst[1][c][1] = psi_p_f_tst[t][z][y][xm / 2][1][c][1] +
                                 sdag * (psi_p_f_tst[t][z][y][xm / 2][2][c][0]);

              // TMP(0,c,2)
              tmp_tst[2][c][0] = psi_p_f_tst[t][z][y][xm / 2][2][c][0] +
                                 sdag * (psi_p_f_tst[t][z][y][xm / 2][1][c][1]);

              // TMP(1,c,2)
              tmp_tst[2][c][1] =
                  psi_p_f_tst[t][z][y][xm / 2][2][c][1] +
                  sdag * (-psi_p_f_tst[t][z][y][xm / 2][1][c][0]);

              // TMP(0,c,3)
              tmp_tst[3][c][0] = psi_p_f_tst[t][z][y][xm / 2][3][c][0] +
                                 sdag * (psi_p_f_tst[t][z][y][xm / 2][0][c][1]);

              // TMP(1,c,3)
              tmp_tst[3][c][1] =
                  psi_p_f_tst[t][z][y][xm / 2][3][c][1] +
                  sdag * (-psi_p_f_tst[t][z][y][xm / 2][0][c][0]);
            }

            /* multiply by U_mu */
            mu = 0;

            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][0]
                chi_tmp[c * 2 + s * 3 * 2] +=
                    (u_p_f_tst[cb][t][z][y][xm / 2][mu][c][0][0] *
                         tmp_tst[s][0][0]

                     + u_p_f_tst[cb][t][z][y][xm / 2][mu][c][1][0] *
                           tmp_tst[s][1][0]

                     + u_p_f_tst[cb][t][z][y][xm / 2][mu][c][2][0] *
                           tmp_tst[s][2][0]

                     + u_p_f_tst[cb][t][z][y][xm / 2][mu][c][0][1] *
                           tmp_tst[s][0][1]

                     + u_p_f_tst[cb][t][z][y][xm / 2][mu][c][1][1] *
                           tmp_tst[s][1][1]

                     + u_p_f_tst[cb][t][z][y][xm / 2][mu][c][2][1] *
                           tmp_tst[s][2][1]);

                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][1]
                chi_tmp[c * 2 + s * 3 * 2 + 1] +=
                    (u_p_f_tst[cb][t][z][y][xm / 2][mu][c][0][0] *
                         tmp_tst[s][0][1]

                     + u_p_f_tst[cb][t][z][y][xm / 2][mu][c][1][0] *
                           tmp_tst[s][1][1]

                     + u_p_f_tst[cb][t][z][y][xm / 2][mu][c][2][0] *
                           tmp_tst[s][2][1]

                     - u_p_f_tst[cb][t][z][y][xm / 2][mu][c][0][1] *
                           tmp_tst[s][0][0]

                     - u_p_f_tst[cb][t][z][y][xm / 2][mu][c][1][1] *
                           tmp_tst[s][1][0]

                     - u_p_f_tst[cb][t][z][y][xm / 2][mu][c][2][1] *
                           tmp_tst[s][2][0]);
              }
            }

            /* 1+gamma_1 */
            /*-----------*/

            for (int c = 0; c < 3; c++) {
              // TMP(0,c,0)
              tmp_tst[0][c][0] =
                  psi_p_f_tst[t][z][ym][x / 2][0][c][0] +
                  sdag * (-psi_p_f_tst[t][z][ym][x / 2][3][c][0]);

              // TMP(1,c,0)
              tmp_tst[0][c][1] =
                  psi_p_f_tst[t][z][ym][x / 2][0][c][1] +
                  sdag * (-psi_p_f_tst[t][z][ym][x / 2][3][c][1]);

              // TMP(0,c,1)
              tmp_tst[1][c][0] = psi_p_f_tst[t][z][ym][x / 2][1][c][0] +
                                 sdag * (psi_p_f_tst[t][z][ym][x / 2][2][c][0]);

              // TMP(1,c,1)
              tmp_tst[1][c][1] = psi_p_f_tst[t][z][ym][x / 2][1][c][1] +
                                 sdag * (psi_p_f_tst[t][z][ym][x / 2][2][c][1]);

              // TMP(0,c,2)
              tmp_tst[2][c][0] = psi_p_f_tst[t][z][ym][x / 2][2][c][0] +
                                 sdag * (psi_p_f_tst[t][z][ym][x / 2][1][c][0]);

              // TMP(1,c,2)
              tmp_tst[2][c][1] = psi_p_f_tst[t][z][ym][x / 2][2][c][1] +
                                 sdag * (psi_p_f_tst[t][z][ym][x / 2][1][c][1]);

              // TMP(0,c,3)
              tmp_tst[3][c][0] =
                  psi_p_f_tst[t][z][ym][x / 2][3][c][0] +
                  sdag * (-psi_p_f_tst[t][z][ym][x / 2][0][c][0]);

              // TMP(1,c,3)
              tmp_tst[3][c][1] =
                  psi_p_f_tst[t][z][ym][x / 2][3][c][1] +
                  sdag * (-psi_p_f_tst[t][z][ym][x / 2][0][c][1]);
            }

            /* multiply by U_mu */
            mu = 1;

            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][0]
                chi_tmp[c * 2 + s * 3 * 2] +=
                    (u_p_f_tst[cb][t][z][ym][x / 2][mu][c][0][0] *
                         tmp_tst[s][0][0] // TMP(0,0,s)

                     + u_p_f_tst[cb][t][z][ym][x / 2][mu][c][1][0] *
                           tmp_tst[s][1][0] // TMP(0,1,s)

                     + u_p_f_tst[cb][t][z][ym][x / 2][mu][c][2][0] *
                           tmp_tst[s][2][0] // TMP(0,2,s)

                     + u_p_f_tst[cb][t][z][ym][x / 2][mu][c][0][1] *
                           tmp_tst[s][0][1] // TMP(1,0,s)

                     + u_p_f_tst[cb][t][z][ym][x / 2][mu][c][1][1] *
                           tmp_tst[s][1][1] // TMP(1,1,s)

                     + u_p_f_tst[cb][t][z][ym][x / 2][mu][c][2][1] *
                           tmp_tst[s][2][1]); ////TMP(1,2,s) );

                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][1]
                chi_tmp[c * 2 + s * 3 * 2 + 1] +=
                    (u_p_f_tst[cb][t][z][ym][x / 2][mu][c][0][0] *
                         tmp_tst[s][0][1] // TMP(1,0,s)

                     + u_p_f_tst[cb][t][z][ym][x / 2][mu][c][1][0] *
                           tmp_tst[s][1][1] // TMP(1,1,s)

                     + u_p_f_tst[cb][t][z][ym][x / 2][mu][c][2][0] *
                           tmp_tst[s][2][1] // TMP(1,2,s)

                     - u_p_f_tst[cb][t][z][ym][x / 2][mu][c][0][1] *
                           tmp_tst[s][0][0] // TMP(0,0,s)

                     - u_p_f_tst[cb][t][z][ym][x / 2][mu][c][1][1] *
                           tmp_tst[s][1][0] // TMP(0,1,s)

                     - u_p_f_tst[cb][t][z][ym][x / 2][mu][c][2][1] *
                           tmp_tst[s][2][0]); // TMP(0,2,s) );
              }
            }

            /* 1+gamma_2 */
            /*-----------*/

            for (int c = 0; c < 3; c++) {
              // TMP(0,c,0)
              tmp_tst[0][c][0] =
                  psi_p_f_tst[t][zm][y][x / 2][0][c][0] // PSI(0,c,0)
                  +
                  sdag *
                      (-psi_p_f_tst[t][zm][y][x / 2][2][c][1]); // PSI(1,c,2) );

              // TMP(1,c,0)
              tmp_tst[0][c][1] =
                  psi_p_f_tst[t][zm][y][x / 2][0][c][1] // PSI(1,c,0)
                  +
                  sdag *
                      (psi_p_f_tst[t][zm][y][x / 2][2][c][0]); // PSI(0,c,2) );

              // TMP(0,c,1)
              tmp_tst[1][c][0] =
                  psi_p_f_tst[t][zm][y][x / 2][1][c][0] // PSI(0,c,1)
                  +
                  sdag *
                      (psi_p_f_tst[t][zm][y][x / 2][3][c][1]); // PSI(1,c,3) );

              // TMP(1,c,1)
              tmp_tst[1][c][1] =
                  psi_p_f_tst[t][zm][y][x / 2][1][c][1] // PSI(1,c,1)
                  +
                  sdag *
                      (-psi_p_f_tst[t][zm][y][x / 2][3][c][0]); // PSI(0,c,3) );

              // TMP(0,c,2)
              tmp_tst[2][c][0] =
                  psi_p_f_tst[t][zm][y][x / 2][2][c][0] // PSI(0,c,2)
                  +
                  sdag *
                      (psi_p_f_tst[t][zm][y][x / 2][0][c][1]); // PSI(1,c,0) );

              // TMP(1,c,2)
              tmp_tst[2][c][1] =
                  psi_p_f_tst[t][zm][y][x / 2][2][c][1] // PSI(1,c,2)
                  +
                  sdag *
                      (-psi_p_f_tst[t][zm][y][x / 2][0][c][0]); // PSI(0,c,0) );

              // TMP(0,c,3)
              tmp_tst[3][c][0] =
                  psi_p_f_tst[t][zm][y][x / 2][3][c][0] // PSI(0,c,3)
                  +
                  sdag *
                      (-psi_p_f_tst[t][zm][y][x / 2][1][c][1]); // PSI(1,c,1) );

              // TMP(1,c,3)
              tmp_tst[3][c][1] =
                  psi_p_f_tst[t][zm][y][x / 2][3][c][1] // PSI(1,c,3)
                  +
                  sdag *
                      (psi_p_f_tst[t][zm][y][x / 2][1][c][0]); // PSI(0,c,1) );
            }

            /* multiply by U_mu */

            mu = 2;

            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][0]
                chi_tmp[c * 2 + s * 3 * 2] +=
                    (u_p_f_tst[cb][t][zm][y][x / 2][mu][c][0][0] *
                         tmp_tst[s][0][0] // TMP(0,0,s)

                     + u_p_f_tst[cb][t][zm][y][x / 2][mu][c][1][0] *
                           tmp_tst[s][1][0] // TMP(0,1,s)

                     + u_p_f_tst[cb][t][zm][y][x / 2][mu][c][2][0] *
                           tmp_tst[s][2][0] // TMP(0,2,s)

                     + u_p_f_tst[cb][t][zm][y][x / 2][mu][c][0][1] *
                           tmp_tst[s][0][1] // TMP(1,0,s)

                     + u_p_f_tst[cb][t][zm][y][x / 2][mu][c][1][1] *
                           tmp_tst[s][1][1] // TMP(1,1,s)

                     + u_p_f_tst[cb][t][zm][y][x / 2][mu][c][2][1] *
                           tmp_tst[s][2][1]); ////TMP(1,2,s) );

                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][1]
                chi_tmp[c * 2 + s * 3 * 2 + 1] +=
                    (u_p_f_tst[cb][t][zm][y][x / 2][mu][c][0][0] *
                         tmp_tst[s][0][1] // TMP(1,0,s)

                     + u_p_f_tst[cb][t][zm][y][x / 2][mu][c][1][0] *
                           tmp_tst[s][1][1] // TMP(1,1,s)

                     + u_p_f_tst[cb][t][zm][y][x / 2][mu][c][2][0] *
                           tmp_tst[s][2][1] // TMP(1,2,s)

                     - u_p_f_tst[cb][t][zm][y][x / 2][mu][c][0][1] *
                           tmp_tst[s][0][0] // TMP(0,0,s)

                     - u_p_f_tst[cb][t][zm][y][x / 2][mu][c][1][1] *
                           tmp_tst[s][1][0] // TMP(0,1,s)

                     - u_p_f_tst[cb][t][zm][y][x / 2][mu][c][2][1] *
                           tmp_tst[s][2][0]); // TMP(0,2,s) );
              }
            }

            /* 1+gamma_3 */
            /*-----------*/

            for (int c = 0; c < 3; c++) {
              // TMP(0,c,0)
              tmp_tst[0][c][0] =
                  psi_p_f_tst[tm][z][y][x / 2][0][c][0] // PSI(0,c,0)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][2][c][0]); // PSI(0,c,2) );

              // TMP(1,c,0)
              tmp_tst[0][c][1] =
                  psi_p_f_tst[tm][z][y][x / 2][0][c][1] // PSI(1,c,0)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][2][c][1]); // PSI(1,c,2) );

              // TMP(0,c,1)
              tmp_tst[1][c][0] =
                  psi_p_f_tst[tm][z][y][x / 2][1][c][0] // PSI(0,c,1)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][3][c][0]); // PSI(0,c,3) );

              // TMP(1,c,1)
              tmp_tst[1][c][1] =
                  psi_p_f_tst[tm][z][y][x / 2][1][c][1] // PSI(1,c,1)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][3][c][1]); // PSI(1,c,3) );

              // TMP(0,c,2)
              tmp_tst[2][c][0] =
                  psi_p_f_tst[tm][z][y][x / 2][2][c][0] // PSI(0,c,2)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][0][c][0]); // PSI(0,c,0) );

              // TMP(1,c,2)
              tmp_tst[2][c][1] =
                  psi_p_f_tst[tm][z][y][x / 2][2][c][1] // PSI(1,c,2)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][0][c][1]); // PSI(1,c,0) );

              // TMP(0,c,3)
              tmp_tst[3][c][0] =
                  psi_p_f_tst[tm][z][y][x / 2][3][c][0] // PSI(0,c,3)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][1][c][0]); // PSI(0,c,1) );

              // TMP(1,c,3)
              tmp_tst[3][c][1] =
                  psi_p_f_tst[tm][z][y][x / 2][3][c][1] // PSI(1,c,3)
                  +
                  sdag *
                      (psi_p_f_tst[tm][z][y][x / 2][1][c][1]); // PSI(1,c,1) );
            }

            /* multiply by U_mu */
            mu = 3;
            for (int s = 0; s < 4; s++) {
              for (int c = 0; c < 3; c++) {
                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][0]
                chi_tmp[c * 2 + s * 3 * 2] +=
                    (u_p_f_tst[cb][tm][z][y][x / 2][mu][c][0][0] *
                         tmp_tst[s][0][0] // TMP(0,0,s)

                     + u_p_f_tst[cb][tm][z][y][x / 2][mu][c][1][0] *
                           tmp_tst[s][1][0] // TMP(0,1,s)

                     + u_p_f_tst[cb][tm][z][y][x / 2][mu][c][2][0] *
                           tmp_tst[s][2][0] // TMP(0,2,s)

                     + u_p_f_tst[cb][tm][z][y][x / 2][mu][c][0][1] *
                           tmp_tst[s][0][1] // TMP(1,0,s)

                     + u_p_f_tst[cb][tm][z][y][x / 2][mu][c][1][1] *
                           tmp_tst[s][1][1] // TMP(1,1,s)

                     + u_p_f_tst[cb][tm][z][y][x / 2][mu][c][2][1] *
                           tmp_tst[s][2][1]); ////TMP(1,2,s) );

                // chi_p[t-1][z-1][y-1][(x-2)/2][s][c][1]
                chi_tmp[c * 2 + s * 3 * 2 + 1] +=
                    (u_p_f_tst[cb][tm][z][y][x / 2][mu][c][0][0] *
                         tmp_tst[s][0][1] // TMP(1,0,s)

                     + u_p_f_tst[cb][tm][z][y][x / 2][mu][c][1][0] *
                           tmp_tst[s][1][1] // TMP(1,1,s)

                     + u_p_f_tst[cb][tm][z][y][x / 2][mu][c][2][0] *
                           tmp_tst[s][2][1] // TMP(1,2,s)

                     - u_p_f_tst[cb][tm][z][y][x / 2][mu][c][0][1] *
                           tmp_tst[s][0][0] // TMP(0,0,s)

                     - u_p_f_tst[cb][tm][z][y][x / 2][mu][c][1][1] *
                           tmp_tst[s][1][0] // TMP(0,1,s)

                     - u_p_f_tst[cb][tm][z][y][x / 2][mu][c][2][1] *
                           tmp_tst[s][2][0]); // TMP(0,2,s) );

                chi_p[t - 1][z - 1][y - 1][(x - 2) / 2][s][c][0] =
                    chi_tmp[c * 2 + s * 3 * 2];
                chi_p[t - 1][z - 1][y - 1][(x - 2) / 2][s][c][1] =
                    chi_tmp[c * 2 + s * 3 * 2 + 1];
              }
            }
          } // end if parity == cbn
        }   // end t loop
      }     // end z loop
    }       // end y loop
  }
#pragma omp target exit data map(                                              \
    from                                                                       \
    : chi_p [0:64] [0:16] [0:48] [0:16] [0:4] [0:3] [0:2],                     \
      u_p_f_tst [0:2] [0:65] [0:17] [0:49] [0:17] [0:4] [0:3] [0:3] [0:2],     \
      psi_p_f_tst [0:66] [0:18] [0:50] [0:18] [0:4] [0:3] [0:2])
  gettimeofday(&tv2, NULL);
  long end = (tv2.tv_sec * 1000000 + tv2.tv_usec);
  fprintf(fp,
          "wilson_dslash_kernel_parPos5_distCol3_parCol1_static_memcpy,%ld\n",
          (end - start));
  // end x loop
}
