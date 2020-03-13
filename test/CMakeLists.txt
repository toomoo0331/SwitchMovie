include_directories(../src)
include_directories(../src/3party)

#add_executable(test_planeDistance test_planeDistance.cpp)
#target_link_libraries(test_planeDistance util sw)
#install(TARGETS test_planeDistance DESTINATION bin)

add_executable(save_movies save_movies.cpp)
install(TARGETS save_movies DESTINATION bin)

add_executable(camera_calibration camera_calibration.cpp)
install(TARGETS camera_calibration DESTINATION bin)

add_executable(set_plane set_plane.cpp)
install(TARGETS set_plane DESTINATION bin)

add_executable(test_SwitchMovie2 test_SwitchMovie2.cpp)
target_link_libraries(test_SwitchMovie2 sw)
install(TARGETS test_SwitchMovie2 DESTINATION bin)

add_executable(combining_movie combining_movie.cpp)
install(TARGETS combining_movie DESTINATION bin)

add_executable(handtracker handtracker.cpp)
target_link_libraries(handtracker hd)
install(TARGETS handtracker DESTINATION bin)

add_executable(switch_movie main.cpp)
target_link_libraries(switch_movie sw util)
install(TARGETS switch_movie DESTINATION bin)

add_executable(switch_movie_csv switch_movie_csv.cpp)
target_link_libraries(switch_movie_csv sw util)
install(TARGETS switch_movie_csv DESTINATION bin)

add_executable(crop_movies crop_movies.cpp)
install(TARGETS crop_movies DESTINATION bin)

add_executable(merge_movie merge_movie.cpp)
install(TARGETS merge_movie DESTINATION bin)

add_executable(rotate rotate_movie.cpp)
install(TARGETS rotate DESTINATION bin)

add_executable(test_model test_model.cpp)
target_link_libraries(test_model hd util)
install(TARGETS test_model DESTINATION bin)

add_executable(test_brightness test_brightness.cpp)
install(TARGETS test_brightness DESTINATION bin)

add_executable(test_light test_light.cpp)
target_link_libraries(test_light sw util)
install(TARGETS test_light DESTINATION bin)

add_executable(make_human make_human.cpp)
install(TARGETS make_human DESTINATION bin)

