void dlib_svm_test3()
{
    typedef dlib::matrix<double, 0, 1> sample_type;
    typedef dlib::linear_kernel<sample_type> kernel_type;

    dlib::svm_pegasos<kernel_type> pegasos;

    int n_iter= 10000;
    for(double lambda= 10e-10;lambda<=10e10;lambda*=10)
    {
        cout << lambda <<endl;

        pegasos.set_lambda(lambda);
        pegasos.set_kernel(kernel_type());

        std::vector<sample_type> samples(6);
        sample_type sample;
        sample.set_size(1);

        sample(0)= 188.0d;
        samples[0]= sample;
        sample(0)= 168.0d;
        samples[1]= sample;
        sample(0)= 191.0d;
        samples[2]= sample;
        sample(0)= 150.0d;
        samples[3]= sample;
        sample(0)= 154.0d;
        samples[4]= sample;
        sample(0)= 124.0d;
        samples[5]= sample;

        for(int iter=0;iter<n_iter;++iter)
        {
            pegasos.train(samples[0],+1);
            pegasos.train(samples[1],+1);
            pegasos.train(samples[2],+1);
            pegasos.train(samples[3],-1);
            pegasos.train(samples[4],-1);
            pegasos.train(samples[5],-1);

            if(     sign(pegasos(samples[0]))==+1 &&
                    sign(pegasos(samples[1]))==+1 &&
                    sign(pegasos(samples[2]))==+1 &&
                    sign(pegasos(samples[3]))==-1 &&
                    sign(pegasos(samples[4]))==-1 &&
                    sign(pegasos(samples[5]))==-1 )
            {
                cout<<"OK."<<endl;
            }
        }

        cout << pegasos(samples[0]) << endl;
        cout << pegasos(samples[1]) << endl;
        cout << pegasos(samples[2]) << endl;
        cout << pegasos(samples[3]) << endl;
        cout << pegasos(samples[4]) << endl;
        cout << pegasos(samples[5]) << endl;

        pegasos.clear();
    }
    cout<<"done!"<<endl;
}
