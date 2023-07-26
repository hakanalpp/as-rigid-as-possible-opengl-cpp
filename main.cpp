#include "src/window.h"

auto read_video_data(char *video_data_path) -> tuple<vector<int>, vector<int>, vector<tuple<double, double>>>
{

    FILE *fPtr = fopen(video_data_path, "r");
    char str[334];

    int left_foot_indexes_len;

    fscanf(fPtr, "%s %d", &str, &left_foot_indexes_len);

    vector<int> left_foot_indexes;

    for (int i = 0; i < left_foot_indexes_len; i++)
    {
        int temp;
        fscanf(fPtr, "%d", &temp);
        left_foot_indexes.push_back(temp);
    }

    int right_foot_indexes_len;

    fscanf(fPtr, "%s %d", &str, &right_foot_indexes_len);

    vector<int> right_foot_indexes;

    for (int i = 0; i < right_foot_indexes_len; i++)
    {
        int temp;
        fscanf(fPtr, "%d", &temp);
        right_foot_indexes.push_back(temp);
    }

    int frame_count;

    fscanf(fPtr, "%s %d", &str, &frame_count);

    vector<tuple<double, double>> frame_offsets;

    for (int i = 0; i < frame_count; i++)
    {
        double temp, temp2;
        fscanf(fPtr, "%lf %lf", &temp, &temp2);
        frame_offsets.push_back(make_tuple(temp, temp2));
    }

    fclose(fPtr);

    return {left_foot_indexes, right_foot_indexes, frame_offsets};
}

int main_video_maker(char *mesh_path, char *video_path)
{

    auto [left_foot_indexes, right_foot_indexes, frame_offsets] = read_video_data(video_path);

    // for(int i=0;i<frame_offsets.size();i++)
    //{
    //	cout<<get<0>(frame_offsets[i]) << " "<< get<1>(frame_offsets[i]) <<endl;
    // }

    vector<int> bottom_points_indexes{290, 300, 314, 315, 331, 332, 356, 357, 358, 576, 577, 584, 587,
                                      588, 589, 595, 596, 603, 604, 610, 613, 614, 615, 616, 617, 618,
                                      619};

    MeshArap mesh = MeshArap();

    mesh.loadOff(mesh_path);

    int iteration_num = 20;
    int frequency_count = 6;

    mesh.print_off("out.off");

    for (int j = 0; j < frequency_count; j++)
    {
        for (int i = 0; i < frame_offsets.size(); i++)
        {

            cout << "frame no: " << j * 20 + i + 1 << endl;
            mesh.addControlPointsVideo2(left_foot_indexes, right_foot_indexes, frame_offsets, i, j % 2);
            // addControlPointsCactus(*mesh, bottom_points_indexes, 591);
            mesh.ARAP(iteration_num);

            mesh.print_off("out" + to_string(j * 20 + i + 1) + ".off");

            mesh.mesh_frame_update();

            return 0;

            // Mesh* mesh = new Mesh();

            // string mesh_path = "out"+to_string(i+1)+".off";

            // mesh->loadOff(mesh_path.data());
        }
    }

    // addControlPoints(*mesh);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1 || (strcmp(argv[1], "video") == 0 && argc < 4))
    {
        std::cout << "Please use it with one of the two options below.\n"
                  << std::endl;
        std::cout << "./arap_cpp <mesh_obj.obj>" << std::endl;
        std::cout << "./arap_cpp video <mesh_path> <video_path> (This one is for our internal video generation. No need to test it.)" << std::endl;
        return -1;
    }
    if (strcmp(argv[1], "video") == 0)
    {
        std::cout << "video mode\n"
                  << std::endl;
        main_video_maker(argv[2], argv[3]);
        return 0;
    }

    Mesh mesh = Mesh(argv[1]);
    MeshArap meshArap = MeshArap();

    meshArap.loadOff(argv[1]);

    Scene scene = Scene(mesh, meshArap);

    Window window = Window(scene);

    window.launch();
    return 0;
}
