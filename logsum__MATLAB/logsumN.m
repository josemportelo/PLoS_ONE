function logsum=logsumN(a,t,n1,n2,bit_reduction_factor,logsum_str)
    if (length(a)==1)
        logsum=a(1);
        return;
    end
    
    if (length(a)==2)
        logsum=logsum2(a(1),a(2),t,n1,n2,bit_reduction_factor,logsum_str);
        return;
    end
    
    odd_number_of_inputs=0;
    if (rem(length(a),2)==1)
        odd_number_of_inputs=1;
    end
    
    new_a=zeros(1,floor(length(a)/2));
    for i=1:floor(length(a)/2)
        new_a(i)=logsum2(a(2*i-1),a(2*i),t,n1,n2,bit_reduction_factor,logsum_str);
    end
    
    if(odd_number_of_inputs)
        new_a(floor(length(a)/2)+1)=a(length(a));
    end
    
    logsum=logsumN(new_a,t,n1,n2,bit_reduction_factor,logsum_str);
end
