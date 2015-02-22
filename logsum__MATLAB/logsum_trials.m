function output=logsum_trials(num_inputs,num_bits,scale_num_bits,num_segments,num_samples)
    %%%%%%%%%%%%%%%%
    % initialization
    %%%%%%%%%%%%%%%%
    display('--- initialization ---')

    factor__x_min=3; % factor__x_min: fixed positive integer ; same as 'scale_num_bits_x' from 'piecewise_approximation.m'
    x_min=-2^(factor__x_min-1)+0.5; % slightly larger than -2^(factor__x_min-1) ; depends on 'x_min' from 'piecewise_approximation.m'
    x_max=-x_min;
    % num_samples=10000;

    % num_inputs=2
    % num_segments=64
    % num_bits=16
    % scale_num_bits=3

    scaling_factor=(2^num_bits)/(2^factor__x_min)
    scaling_factor_t=scaling_factor
    scaling_factor_n_SQ=scaling_factor
    scaling_factor_n1_RQ=2^num_bits
    scaling_factor_n2_RQ=scaling_factor_t*scaling_factor_n1_RQ

    lookup_table_dir='..\logsum\look-up_table\';
    t_str=sprintf('%s/t.num_segments_%d.scaling_factor_%ld.num_bits_%d.scale_num_bits_%d.txt',...
        lookup_table_dir,num_segments,scaling_factor_t,num_bits,scale_num_bits);
    full__t__LT=load(t_str);
    t__LT=full__t__LT(:,1);
    n_SQ_str=sprintf('%s/n_SQ.num_segments_%d.scaling_factor_%s.num_bits_%d.scale_num_bits_%d.txt',...
        lookup_table_dir,num_segments,int2str(scaling_factor_n_SQ),num_bits,scale_num_bits);
    full__n_SQ__LT=load(n_SQ_str);
    n_SQ__LT=full__n_SQ__LT(:,1);
    n1_RQ_str=sprintf('%s/n1_RQ.num_segments_%d.scaling_factor_%s.num_bits_%d.scale_num_bits_%d.txt',...
        lookup_table_dir,num_segments,int2str(scaling_factor_n1_RQ),num_bits,scale_num_bits);
    full__n1_RQ__LT=load(n1_RQ_str);
    n1_RQ__LT=full__n1_RQ__LT(:,1);
    n2_RQ_str=sprintf('%s/n2_RQ.num_segments_%d.scaling_factor_%s.num_bits_%d+%d.scale_num_bits_%d.txt',...
        lookup_table_dir,num_segments,int2str(scaling_factor_n2_RQ),num_bits,num_bits,scale_num_bits);
    full__n2_RQ__LT=load(n2_RQ_str);
    n2_RQ__LT=full__n2_RQ__LT(:,1);

    %%%%%%%%%%%%%%%%%%%%%%
    % generate random data
    %%%%%%%%%%%%%%%%%%%%%%
    display('--- generate random data ---')

    x=rand(num_samples,num_inputs)*(x_max-x_min)-(x_max-x_min)/2;
    x_GC=(round(x*scaling_factor));%/scaling_factor;

    %%%%%%%%%%%%%%%%
    % compute logsum
    %%%%%%%%%%%%%%%%
    display('--- compute logsum ---')

    % exact computation
    display('   | exact computation |')

    logsum_exact=zeros(num_samples,1);
    for i=1:num_samples
        max_input_logsum=max(x(i,:));
        for j=1:num_inputs
            logsum_exact(i,1)=logsum_exact(i,1)+exp(x(i,j)-max_input_logsum);
        end
        logsum_exact(i,1)=max_input_logsum+log(logsum_exact(i,1));
    end

    % GC computation
    display('   | GC computation |')

    logsum_GC__SQ=zeros(num_samples,1);
    logsum_GC__RQ=zeros(num_samples,1);
    for i=1:num_samples
        if (mod(i,1000)==0)
            i
        end

        logsum_GC__SQ(i,1)=logsumN(x_GC(i,:),t__LT,n_SQ__LT,0,0,'SQ');
        logsum_GC__RQ(i,1)=logsumN(x_GC(i,:),t__LT,n1_RQ__LT,n2_RQ__LT,scaling_factor_n1_RQ,'RQ');
    end
    logsum_GC_scaled__SQ=logsum_GC__SQ./scaling_factor;
    logsum_GC_scaled__RQ=logsum_GC__RQ./scaling_factor;

    x_GC=x_GC
    logsum_results=[logsum_exact*scaling_factor logsum_GC__SQ logsum_GC__RQ]
    logsum_results__GC=[logsum_exact logsum_GC_scaled__SQ logsum_GC_scaled__RQ]

    %%%%%%%%%%%%%%%
    % compute error
    %%%%%%%%%%%%%%%
    display('--- compute mean error ---')

    error_SQ=abs(logsum_GC__SQ/scaling_factor-logsum_exact);
    max_error_SQ=max(error_SQ);
    mean_error_SQ=mean(error_SQ);
    var_error_SQ=var(error_SQ);
    disp(sprintf('  SQ - max: %0.6f ; mean: %0.6f ; var: %0.9f',max_error_SQ,mean_error_SQ,var_error_SQ))

    % percent_error_SQ=abs(error_SQ./logsum_exact)*100;
    % percent_max_error_SQ=max(percent_error_SQ);
    % percent_mean_error_SQ=mean(percent_error_SQ);
    % percent_var_error_SQ=var(percent_error_SQ);
    % disp(sprintf('       %%max: %0.6f%% ; %%mean: %0.6f%% ; %%var: %0.9f%%',percent_max_error_SQ,percent_mean_error_SQ,percent_var_error_SQ))

    error_RQ=abs(logsum_GC__RQ/scaling_factor-logsum_exact);
    max_error_RQ=max(error_RQ);
    mean_error_RQ=mean(error_RQ);
    var_error_RQ=var(error_RQ);
    disp(sprintf('  RQ - max: %0.6f ; mean: %0.6f ; var: %0.9f',max_error_RQ,mean_error_RQ,var_error_RQ))

    % percent_error_RQ=abs(error_RQ./logsum_exact)*100;
    % percent_max_error_RQ=max(percent_error_RQ);
    % percent_mean_error_RQ=mean(percent_error_RQ);
    % percent_var_error_RQ=var(percent_error_RQ);
    % disp(sprintf('       %%max: %0.6f%% ; %%mean: %0.6f%% ; %%var: %0.9f%%',percent_max_error_RQ,percent_mean_error_RQ,percent_var_error_RQ))

    %check=[x x_GC logsum_exact logsum_GC__SQ/scaling_factor logsum_GC__RQ/scaling_factor error_SQ error_RQ];% percent_error_SQ percent_error_RQ];

    %%%%%%%%%%%%%%%%%%%%%%
    % save results to file
    %%%%%%%%%%%%%%%%%%%%%%
    display('--- save results to file ---')

    output_dir=sprintf('logsum_error');
    system_str=['mkdir ',output_dir];
    system(system_str);

    file_name=sprintf('%s/logsum_error.num_inputs_%d.num_segments_%d.num_bits_%d.txt',output_dir,num_inputs,num_segments,num_bits);
    file=fopen(file_name,'w');
    fprintf(file,'||| num inputs: %d | num segments: %d | num bits: %d |||\n',num_inputs,num_segments,num_bits);
    fprintf(file,'SQ - max: %0.6f ; mean: %0.6f ; var: %0.9f\n',max_error_SQ,mean_error_SQ,var_error_SQ);
    fprintf(file,'RQ - max: %0.6f ; mean: %0.6f ; var: %0.9f\n',max_error_RQ,mean_error_RQ,var_error_RQ);
    fclose(file);
end