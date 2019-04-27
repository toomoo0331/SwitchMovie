//
// Created by kei666 on 18/07/16.
//

#ifndef PROJECT_POINTS_H
#define PROJECT_POINTS_H

#include <map>

#include <opencv2/opencv.hpp>

namespace calibration {

class Points {
public:
    typedef int ImageID;
    typedef int TrackID;
    typedef std::map<std::pair<ImageID, TrackID>, const cv::Point2f> PointMap;

    void insert(const ImageID imageIndex, const TrackID trackIndex, const cv::Point2f &point) {
        point_map_.insert(std::make_pair(std::make_pair(imageIndex, trackIndex), point));
        images_.insert(imageIndex);
        tracks_.insert(trackIndex);
    }

    void remove(const ImageID imageIndex, const TrackID trackIndex) {
        point_map_.erase(std::make_pair(imageIndex, trackIndex));
    }

    void clear() {
        point_map_.clear();
        images_.clear();
        tracks_.clear();
    }

    const cv::Point2f *get(const ImageID imageIndex, const TrackID trackIndex) const {
        typename PointMap::const_iterator it =
                point_map_.find(std::make_pair(imageIndex, trackIndex));
        if (it != point_map_.end()) {
            return &(it->second);
        }
        return NULL;
    }

    int getImagePoints(const ImageID imageIndex, std::vector<cv::Point2f> *points) const {
        int n = 0;
        typename PointMap::const_iterator it;
        for (it = point_map_.begin(); it != point_map_.end(); ++it) {
            if (it->first.first == imageIndex) {
                points->push_back(it->second);
                n++;
            }
        }
        return n;
    }

    int getImageTrackNum(const ImageID imageIndex) const {
        int n = 0;
        typename PointMap::const_iterator it;
        for (it = point_map_.begin(); it != point_map_.end(); ++it) {
            if (it->first.first == imageIndex) {
                n++;
            }
        }
        return n;
    }

    int imageNum() const { return images_.size(); }

    int trackNum() const { return tracks_.size(); }

    typedef PointMap::iterator iterator;
    typedef PointMap::const_iterator const_iterator;

    iterator begin() { return point_map_.begin(); }
    const_iterator begin() const { return point_map_.begin(); }
    iterator end() { return point_map_.end(); }
    const_iterator end() const { return point_map_.end(); };

private:
    PointMap point_map_;
    std::set<ImageID> images_;
    std::set<TrackID> tracks_;
};

}


#endif //PROJECT_POINTS_H
