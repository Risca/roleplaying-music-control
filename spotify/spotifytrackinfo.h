#ifndef SPOTIFYTRACKINFO_H
#define SPOTIFYTRACKINFO_H

#include <QString>

struct SpotifyTrackInfo {
    QString name;
    QString URI;
};
Q_DECLARE_METATYPE(SpotifyTrackInfo);
#endif // SPOTIFYTRACKINFO_H

