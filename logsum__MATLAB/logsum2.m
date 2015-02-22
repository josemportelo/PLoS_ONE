function logsum=logsum2(a,b,t,n1,n2,bit_reduction_factor,logsum_str)
    % initialization    
    max_ab=max(a,b);
    min_ab=min(a,b);
    sub_Mm=max_ab-min_ab;
    
    % get index for look-up table
    cmp=sub_Mm>t;
    aux_index=find(cmp==1,1);
    if (isempty(aux_index))
        index=length(t)+1;
    else
        index=aux_index;
    end
    
    % compute logsum
    %   step quantization (SQ)
    if (strcmp(logsum_str,'SQ'))
        logsum=n1(index)+max_ab;
    end
    
    %   ramp quantization (RQ)
    if (strcmp(logsum_str,'RQ'))
        logsum=floor((n1(index)*(-sub_Mm)+n2(index))/bit_reduction_factor)+max_ab;
    end
end
