#pragma  once

#include <string>

static constexpr std::string GetASCIILogo( const std::string *info ) {
    std::string logo;

    logo.append( "               @@@    @@            @@@                          " + info[0] + "\n" );
    logo.append( "             @@@@@  @@@@          @@@@                           " + info[1] + "\n" );
    logo.append( "            @@  @@@@  @         @@ @@         @@@@               " + info[2] + "\n" );
    logo.append( "           @@   @@@@@@@       @@@  @@      @@@ @@                " + info[3] + "\n" );
    logo.append( "         @@@@       @@@      @@@   @@    @@@  @@                 " + info[4] + "\n" );
    logo.append( "       @@             @@     @@     @@  @@    @@                 " + info[5] + "\n" );
    logo.append( "      @@               @@    @@     @@  @@    @@    @@@@@        " + info[6] + "\n" );
    logo.append( "     @@   @@        @@  @    @@      @@@@     @@  @@@@@@         " + info[7] + "\n" );
    logo.append( "     @@ @@              @@@  @@       @@@      @@@@  @@          " + info[8] + "\n" );
    logo.append( "    @@ @@     @@@@@@      @@@ @@       @@      @@@   @@          " + info[9] + "\n" );
    logo.append( "    @@@@        @@@ @@@@@@@@  @@       @@@     @@@    @@         " + info[10] + "\n" );
    logo.append( "    @@@        @@ @@           @@       @@     @@@    @@         " + info[11] + "\n" );
    logo.append( "    @@@       @@   @@           @@      @@     @@      @         " + info[12] + "\n" );
    logo.append( "    @@@      @@      @@@         @@     @@     @       @         " + info[13] + "\n" );
    logo.append( "     @@      @         @@@@       @@    @@    @@      @@         " + info[14] + "\n" );
    logo.append( "      @@    @@            @@@     @@    @     @@      @@         " + info[15] + "\n" );
    logo.append( "      @@@   @@              @@    @@   @@    @@      @@          " + info[16] + "\n" );
    logo.append( "        @@  @@               @@   @@  @@    @@      @@   @       " + info[17] + "\n" );
    logo.append( "         @@  @@    @          @@  @@ @@   @@@     @@@  @@@       " + info[18] + "\n" );
    logo.append( "          @@@@@   @@           @ @@@@@  @@@     @@@@@@@ @        " + info[19] + "\n" );
    logo.append( "           @@@@   @@           @@@@@@@@@     @@@@      @@        " + info[20] + "\n" );
    logo.append( "           @@ @  @@@@          @@@@@@  @@@@@@@       @@@         " + info[21] + "\n" );
    logo.append( "            @ @  @@ @@         @@@@                 @@           " + info[22] + "\n" );
    logo.append( "            @@@  @    @@       @@  @@@           @@@             " + info[23] + "\n" );
    logo.append( "            @@@ @@      @@      @@    @@@@@@@@@@@                " + info[24] + "\n" );
    logo.append( "           @@@ @@  @@@@@@   @    @@@         @@@@@@              " + info[25] + "\n" );
    logo.append( "          @@@@@   @@@@@@@@@         @@@@@@@@@@@@                 " + info[26] + "\n" );

    return logo;
}
